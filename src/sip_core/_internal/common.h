#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stc/random.h"

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/sip_listener_list.h"
#include "sip_core/_internal/sip_strans_hashmap.h"
#include "sip_transp/sip_transp.h"

struct cmsu_SipCore {
  event_loop_t evl;
  sip_transp_t sip_transp;
  list_cmsu_SipListeners sip_lstnrs;
  hmap_cmsu_SipStransMap sip_strans;
};

static inline const char *cmsu_sip_msg_generate_to_tag(void) {
  const uint64_t seed = (uint64_t)time(NULL);
  crand64 rng = crand64_from(seed);

  char random = (char)crand64_uint_r(&rng, 1);

  printf("Random=%d\n", random);

  return 0;
}

static inline cme_error_t
cmsu_sip_msg_create_response_from_request(sip_msg_t request, sip_msg_t *out) {
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

   5. If a request contained a To tag in the request, the To header field in the
   response MUST equal that of the request. However, if the To header field in
   the request did not contain a tag, the URI in the To header field in the
   response MUST equal the URI in the To header field; additionally, the UAS
   MUST add a tag to the To header field in the response (with the exception of
   the 100 (Trying) response, in which a tag MAY be present). This serves to
   identify the UAS that is responding, possibly resulting in a component of a
   dialog ID. The same tag MUST be used for all responses to that request, both
   final and provisional (again excepting the 100 (Trying)). Procedures for the
   generation of tags are defined in Section 19.3.

  */
  sip_msg_t response;
  cme_error_t err;

  err = cmsc_sipmsg_create_with_buf(&response);
  if (err) {
    goto error_out;
  }

  { // Copy `From` header
    struct cmsc_String request_from_uri =
        cmsc_bs_msg_to_string(&request->from.uri, request);

    if (!request_from_uri.len) {
      err = cme_error(ENODATA, "No FROM field in the sip request");
      goto error_response_cleanup;
    }

    struct cmsc_String request_from_tag =
        cmsc_bs_msg_to_string(&request->from.tag, request);

    err = cmsc_sipmsg_insert_from(request_from_uri.len, request_from_uri.buf,
                                  request_from_tag.len, request_from_tag.buf,
                                  response);
    if (err) {
      goto error_response_cleanup;
    }
  }

  { // Copy `Call-ID` header
    struct cmsc_String request_call_id =
        cmsc_bs_msg_to_string(&request->call_id, request);

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
        cmsc_bs_msg_to_string(&request->cseq.method, request);

    if (!request_cseq.len) {
      err = cme_error(ENODATA, "No CSeq field in the sip request");
      goto error_response_cleanup;
    }

    err = cmsc_sipmsg_insert_cseq(request_cseq.len, request_cseq.buf,
                                  request->cseq.seq_number, response);
    if (err) {
      goto error_response_cleanup;
    }
  }

  { // Copy `Via` headers
    struct cmsc_SipHeaderVia *via;
    STAILQ_FOREACH(via, &request->vias, _next) {
      struct cmsc_String proto = cmsc_bs_msg_to_string(&via->proto, request);
      struct cmsc_String sent_by =
          cmsc_bs_msg_to_string(&via->sent_by, request);
      struct cmsc_String addr = cmsc_bs_msg_to_string(&via->addr, request);
      struct cmsc_String branch = cmsc_bs_msg_to_string(&via->branch, request);
      struct cmsc_String received =
          cmsc_bs_msg_to_string(&via->received, request);

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
        cmsc_bs_msg_to_string(&request->to.uri, request);

    if (!request_to_uri.len) {
      err = cme_error(ENODATA, "No To field in the sip request");
      goto error_response_cleanup;
    }

    struct cmsc_String request_to_tag =
        cmsc_bs_msg_to_string(&request->to.tag, request);
    if (!request_to_tag.len) {
      err = cme_error(ENODATA, "No To field in the sip request");
      goto error_response_cleanup;
    }

    cmsu_sip_msg_generate_to_tag();
  }

  *out = response;

  return 0;

error_response_cleanup:
  cmsc_sipmsg_destroy_with_buf(&response);
error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
