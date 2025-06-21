/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UTILS_SIP_MSG_H
#define C_MINILIB_SIP_UA_UTILS_SIP_MSG_H
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "stc/random.h"

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "stc/cstr.h"
#include "utils/csview_ptr.h"

static inline void __SipMessage_destroy(struct cmsc_SipMessage **data) {
  puts(__func__);
  cmsc_sipmsg_destroy(data);
};
static inline struct cmsc_SipMessage *
__SipMessage_clone(struct cmsc_SipMessage *sip_msg) {
  return sip_msg;
};

#define i_type __SipMessagePtr
#define i_key struct cmsc_SipMessage *
#define i_keydrop __SipMessage_destroy
#define i_keyclone __SipMessage_clone
#include "stc/arc.h"

typedef struct __SipMessagePtr sip_msg_t;

static inline cme_error_t sip_msg_parse(csview_ptr_t buf, sip_msg_t *out) {
  puts(__func__);
  cme_error_t err;

  struct cmsc_SipMessage *msg;
  err = cmsc_parse_sip(buf.get->size, buf.get->buf, &msg);
  if (err) {
    // TO-DO log that malformed sip msg send.
    if (err->code == EINVAL) {
      puts(err->msg);
      puts("malformed msg");
      return 0;
    }
    goto error_out;
  }

  assert(msg != NULL);

  csview_ptr_ref(buf); // Now message holds buf memory
  *out = __SipMessagePtr_from(msg);

  assert(out->get != NULL);

  return 0;

error_out:
  return cme_return(err);
}

static inline sip_msg_t sip_msg_ref(sip_msg_t msgp) {
  __SipMessagePtr_clone(msgp);
  return msgp;
}

static inline void sip_msg_deref(sip_msg_t *msgp) {
  __SipMessagePtr_drop(msgp);
}

static inline csview *sip_msg_get_branch(sip_msg_t msgp, csview *out) {
  struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&(*msgp.get)->vias);
  struct cmsc_String branch = {0};
  if (via) {
    branch = cmsc_bs_msg_to_string(&via->branch, *msgp.get);
  }

  if (!branch.len) {
    goto error_out;
  }

  *out = (csview){.buf = branch.buf, .size = branch.len};

  return out;

error_out:
  *out = (csview){0};
  return NULL;
}

static inline csview *sip_msg_get_method(sip_msg_t msgp, csview *out) {
  struct cmsc_String method = {0};

  method =
      cmsc_bs_msg_to_string(&(*msgp.get)->request_line.sip_method, *msgp.get);

  if (!method.len) {
    goto error_out;
  }

  *out = (csview){.buf = method.buf, .size = method.len};

  return out;

error_out:
  *out = (csview){0};
  return NULL;
}

static inline bool sip_msg_is_request(sip_msg_t msgp) {
  return cmsc_sipmsg_is_field_present(*msgp.get,
                                      cmsc_SupportedSipHeaders_REQUEST_LINE);
}

static inline cme_error_t
__sip_msg_status_add_to_tag(csview to_uri, struct cmsc_SipMessage *sipmsg);

static inline cme_error_t sip_msg_status_from_request(sip_msg_t request,
                                                      uint32_t status_code,
                                                      cstr status,
                                                      sip_msg_t *out) {
  /*
    According RFC 3261 8.2.6.2 Headers And Tags:
     1. The From field of the response MUST equal the From header field of
   the request. (Implemented)

     2. The Call-ID header field of the response MUST equal the Call-ID header
   field of the request. (Implemented)

     3. The CSeq header field of the response MUST equal the CSeq field of the
   request. (Implemented)

     4.The Via header field values in the response MUST equal the Via header
   field values in the request and MUST maintain the same ordering.
   (Implemented)

     5. If a request contained a To tag in the request, the To header field in
   the response MUST equal that of the request. However, if the To header field
   in the request did not contain a tag, the URI in the To header field in the
   response MUST equal the URI in the To header field; additionally, the UAS
   MUST add a tag to the To header field in the response (with the exception of
   the 100 (Trying) response, in which a tag MAY be present). This serves to
   identify the UAS that is responding, possibly resulting in a component of a
   dialog ID. The same tag MUST be used for all responses to that request, both
   final and provisional (again excepting the 100 (Trying)). Procedures for the
   generation of tags are defined in Section 19.3.
  */
  struct cmsc_SipMessage *response;
  cme_error_t err;

  err = cmsc_sipmsg_create_with_buf(&response);
  if (err) {
    goto error_out;
  }

  err =
      cmsc_sipmsg_insert_status_line(strlen("2.0"), "2.0", cstr_size(&status),
                                     cstr_data(&status), status_code, response);
  if (err) {
    goto error_out;
  }

  { // Copy `From` header
    struct cmsc_String request_from_uri =
        cmsc_bs_msg_to_string(&(*request.get)->from.uri, *request.get);

    if (!request_from_uri.len) {
      err = cme_error(ENODATA, "No FROM field in the sip request");
      goto error_response_cleanup;
    }

    struct cmsc_String request_from_tag =
        cmsc_bs_msg_to_string(&(*request.get)->from.tag, *request.get);

    err = cmsc_sipmsg_insert_from(request_from_uri.len, request_from_uri.buf,
                                  request_from_tag.len, request_from_tag.buf,
                                  response);
    if (err) {
      goto error_response_cleanup;
    }
  }

  { // Copy `Call-ID` header
    struct cmsc_String request_call_id =
        cmsc_bs_msg_to_string(&(*request.get)->call_id, *request.get);

    if (!request_call_id.len) {
      err = cme_error(ENODATA, "No Call-ID field in the sip request");
      goto error_response_cleanup;
    }

    err = cmsc_sipmsg_insert_call_id(request_call_id.len, request_call_id.buf,
                                     response);
    if (err) {
      goto error_response_cleanup;
    }
  }

  { // Copy `Cseq` header
    struct cmsc_String request_cseq =
        cmsc_bs_msg_to_string(&(*request.get)->cseq.method, *request.get);

    if (!request_cseq.len) {
      err = cme_error(ENODATA, "No CSeq field in the sip request");
      goto error_response_cleanup;
    }

    err = cmsc_sipmsg_insert_cseq(request_cseq.len, request_cseq.buf,
                                  (*request.get)->cseq.seq_number, response);
    if (err) {
      goto error_response_cleanup;
    }
  }

  { // Copy `Via` headers
    struct cmsc_SipHeaderVia *via;
    STAILQ_FOREACH(via, &(*request.get)->vias, _next) {
      struct cmsc_String proto =
          cmsc_bs_msg_to_string(&via->proto, *request.get);
      struct cmsc_String sent_by =
          cmsc_bs_msg_to_string(&via->sent_by, *request.get);
      struct cmsc_String addr = cmsc_bs_msg_to_string(&via->addr, *request.get);
      struct cmsc_String branch =
          cmsc_bs_msg_to_string(&via->branch, *request.get);
      struct cmsc_String received =
          cmsc_bs_msg_to_string(&via->received, *request.get);

      err = cmsc_sipmsg_insert_via(proto.len, proto.buf, sent_by.len,
                                   sent_by.buf, addr.len, addr.buf, branch.len,
                                   branch.buf, received.len, received.buf,
                                   via->ttl, response);
      if (err) {
        goto error_response_cleanup;
      }
    }
  }

  { // Copy `To` header
    struct cmsc_String request_to_uri =
        cmsc_bs_msg_to_string(&(*request.get)->to.uri, *request.get);

    if (!request_to_uri.len) {
      err = cme_error(ENODATA, "No To field in the sip request");
      goto error_response_cleanup;
    }

    struct cmsc_String request_to_tag =
        cmsc_bs_msg_to_string(&(*request.get)->to.tag, *request.get);
    if (!request_to_tag.len) {
      err = __sip_msg_status_add_to_tag(
          c_sv(request_to_uri.buf, request_to_uri.len), response);
      if (err) {
        goto error_response_cleanup;
      }
    }
  }

  *out = __SipMessagePtr_from(response);

  return 0;

error_response_cleanup:
  cmsc_sipmsg_destroy_with_buf(&response);
error_out:
  return cme_return(err);
}

static inline cme_error_t
__sip_msg_status_add_to_tag(csview to_uri, struct cmsc_SipMessage *sipmsg) {
  const uint32_t seed = (uint32_t)time(NULL);
  crand32 rng = crand32_from(seed);

  char new_tag[10];
  uint32_t chunk_size = 2;
  for (uint32_t i = 0; i < 4; i++) {
    char random = abs((char)crand32_uint_r(&rng, 1));
    sprintf(new_tag + (i * chunk_size), "%.*d", chunk_size, random);
  }

  printf("Random=%.*s\n", 8, new_tag);

  return cmsc_sipmsg_insert_to(to_uri.size, to_uri.buf, 8, new_tag, sipmsg);
}

static inline cme_error_t sip_msg_generate(sip_msg_t sip_msg,
                                           csview_ptr_t *out) {
  struct csview buffer;
  cme_error_t err;

  err = cmsc_generate_sip(*sip_msg.get, (uint32_t *)&buffer.size, &buffer.buf);
  if (err) {
    goto error_out;
  }

  printf("Buggy buffer size: %td\n\n", buffer.size);

  *out = csview_ptr_from(buffer);

  return 0;

error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_SIP_MSG_H
