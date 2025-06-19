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

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "stc/cstr.h"
#include "utils/csview_ptr.h"

static inline void __SipMessage_destroy(struct cmsc_SipMessage **data) {
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

#endif // C_MINILIB_SIP_UA_SIP_MSG_H
