/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SOCKET_H
#define C_MINILIB_SIP_UA_INT_SOCKET_H
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "socket/_internal/common.h"
#include "socket/_internal/udp.h"
#include "socket/socket.h"
#include "utils/common.h"

/******************************************************************************
 *                                Socket                                      *
 ******************************************************************************/
static inline cme_error_t cmsu_Socket_recv(socket_t socket) {
  cme_error_t err;

  switch (socket->type) {
  case SocketType_UDP:
    err = cmsu_SocketUdp_recv(socket->proto);
    break;
  default:
    err = cme_error(EINVAL, "Unsupported socket type");
  }

  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t cmsu_Socket_send(ip_addr_t recver, buffer_t *buf,
                                           socket_t socket) {
  cme_error_t err;
  switch (socket->type) {
  case SocketType_UDP:
    err = cmsu_SocketUdp_send(socket->proto, recver, buf);
    break;
  default:
    err = cme_error(EINVAL, "Unsupported socket type");
  }

  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline int cmsu_Socket_get_fd(socket_t socket) {
  switch (socket->type) {
  case SocketType_UDP:
    return cmsu_SocketUdp_get_fd(socket->proto);
    break;
  default:
    return -1;
  }
}

static inline int cmsu_Socket_cmp(const struct cmsu_Socket *a,
                                  const struct cmsu_Socket *b) {
  if (cmsu_Socket_get_fd((socket_t)a) == cmsu_Socket_get_fd((socket_t)b)) {
    return 0;
  }
  if (cmsu_Socket_get_fd((socket_t)a) > cmsu_Socket_get_fd((socket_t)b)) {
    return 1;
  }
  return -1;
}

#endif // C_MINILIB_SIP_UA_SOCKET_H
