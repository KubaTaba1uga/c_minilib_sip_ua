/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_REQUEST_H
#define C_MINILIB_SIP_UA_INT_SIP_REQUEST_H
#include "c_minilib_error.h"
#include "sip/sip.h"
#include "socket/socket.h"
#include <stdint.h>

/******************************************************************************
 *                                Sip Request                                 *
 ******************************************************************************/
struct cmsu_SipRequest {
  ip_addr_t recver;
  socket_t socket;
  sip_msg_t msg;
  uint32_t id;
};

static inline int cmsu_SipRequest_cmp(const struct cmsu_SipRequest *a,
                                      const struct cmsu_SipRequest *b) {
  if (socket_get_fd(a->socket) == socket_get_fd(b->socket)) {
    return 0;
  }
  if (socket_get_fd(a->socket) > socket_get_fd(b->socket)) {
    return 1;
  }

  return -1;
}

#endif // C_MINILIB_SIP_UA_SIP_REQUEST_H
