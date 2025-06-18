/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_MSG_H
#define C_MINILIB_SIP_UA_SIP_MSG_H

#include "buffer.h"
#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#include "utils/buffer.h"
#include <stdint.h>

typedef struct cmsc_SipMessage *sip_msg_t;

static inline void sip_msg_destroy(void *sip_msg) {
  cmsc_sipmsg_destroy((sip_msg_t *)&sip_msg);
}

#endif // C_MINILIB_SIP_UA_SIP_MSG_H
