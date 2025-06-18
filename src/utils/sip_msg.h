/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_MSG_H
#define C_MINILIB_SIP_UA_SIP_MSG_H
#include <stdint.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "stc/cstr.h"

static inline void __SipMessage_destroy(void *data) {
  cmsc_sipmsg_destroy(data);
};
static inline struct SipMessage
__SipMessage_clone(struct SipMessage udp_socket);

#define i_type sip_msg
#define i_key struct cmsc_SipMessage
#define i_keydrop __SipMessage_destroy
#define i_keyclone __SipMessage_clone
#include "stc/arc.h"

typedef struct sip_msg sip_msg_t;

static inline sip_msg_parse(cstr buf, sip_msg_t *out) {
  csview buf_view = cstr_sv(&buf);
  cme_error_t err;

  err = cmsc_parse_sip(buf_view.size, buf_view.buf, &out->get);
  if (err) {
    // TO-DO log that malformed sip msg send.
    if (err->code == EINVAL) {
      return 0;
    }
    goto error_out;
  }
}
/* typedef struct cmsc_SipMessage *sip_msg_t; */

/* typedef struct cmsc_SipMessage *sip_msg_t; */

/* static inline void sip_msg_destroy(void *sip_msg) { */
/*   cmsc_sipmsg_destroy((sip_msg_t *)&sip_msg); */
/* } */

#endif // C_MINILIB_SIP_UA_SIP_MSG_H
