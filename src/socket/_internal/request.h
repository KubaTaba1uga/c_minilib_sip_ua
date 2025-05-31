/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_REQUEST_H
#define C_MINILIB_SIP_UA_INT_REQUEST_H
#include "c_minilib_error.h"
#include "socket/socket.h"

/******************************************************************************
 *                                Request                                     *
 ******************************************************************************/

struct cmsu_Request {
  socket_t socket;
  ip_addr_t recver;
  buffer_t buffer;
  void *data;
};

static inline int cmsu_Request_cmp(const struct cmsu_Request *a,
                                   const struct cmsu_Request *b) {
  if (socket_get_fd(a->socket) == socket_get_fd(b->socket)) {
    return 0;
  }
  if (socket_get_fd(a->socket) > socket_get_fd(b->socket)) {
    return 1;
  }

  return -1;
}

#endif // C_MINILIB_SIP_UA_REQUEST_H
