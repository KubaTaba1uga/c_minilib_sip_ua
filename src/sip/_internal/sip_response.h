/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_RESPONSE_H
#define C_MINILIB_SIP_UA_INT_SIP_RESPONSE_H
#include "c_minilib_error.h"
#include "sip/sip.h"
#include "socket/socket.h"
#include <stdint.h>

/******************************************************************************
 *                                Sip Response                                 *
 ******************************************************************************/
struct cmsu_SipResponse {
  ip_addr_t sender;
  sip_msg_t msg;
  uint32_t id;
};

static inline int cmsu_SipResponse_cmp(const struct cmsu_SipResponse *a,
                                       const struct cmsu_SipResponse *b) {
  if (a->id == b->id) {
    return 0;
  }
  if (a->id > b->id) {
    return 1;
  }

  return -1;
}

#endif // C_MINILIB_SIP_UA_SIP_RESPONSE_H
