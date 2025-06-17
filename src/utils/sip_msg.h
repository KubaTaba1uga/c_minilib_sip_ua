/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_MSG_H
#define C_MINILIB_SIP_UA_SIP_MSG_H
#include "c_minilib_sip_codec.h"
#include "stc/cstr.h"
#include "stc/csview.h"

static inline void __sip_msg_ptr_destroy(struct cmsc_SipMessage **sipmsg) {
  cmsc_sipmsg_destroy(sipmsg);
}

#define i_type sip_msg_ptr
#define i_key struct cmsc_SipMessage *
#define i_keydrop __sip_msg_ptr_destroy
#include "stc/arc.h"

typedef struct sip_msg_ptr sip_msg_ptr_t;

#endif // C_MINILIB_SIP_UA_SIP_MSG_H
