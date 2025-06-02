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
static inline cme_error_t cmsu_Socket_recv_event_handler(socket_t socket) {
  cme_error_t err;

  switch (socket->proto_type) {
  case SocketProto_UDP:
    err = cmsu_SocketUdp_recv_event_handler(socket->proto);
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

static inline cme_error_t cmsu_Socket_send_event_handler(socket_t socket,
                                                         bool *is_send_done) {
  cme_error_t err;
  switch (socket->proto_type) {
  case SocketProto_UDP:
    err = cmsu_SocketUdp_send_event_handler(socket->proto, is_send_done);
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
  switch (socket->proto_type) {
  case SocketProto_UDP:
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

void cmsu_Socket_destroy(socket_t *socket) {
  if (!socket || !*socket) {
    return;
  }

  switch ((*socket)->proto_type) {
  case SocketProto_UDP:
    cmsu_SocketUdp_destroy((*socket)->proto);
    break;
  default:;
  }

  *socket = NULL;
};

static inline cme_error_t cmsu_Socket_recv_sync(ip_addr_t *sender,
                                                buffer_t *buffer,
                                                struct cmsu_Socket *socket) {
  cme_error_t err;
  switch (socket->proto_type) {
  case SocketProto_UDP:
    err = cmsu_SocketUdp_recv_sync(sender, buffer, socket->proto);
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

static inline cme_error_t cmsu_Socket_send_sync(ip_addr_t *recver,
                                                buffer_t *buffer,
                                                struct cmsu_Socket *socket) {
  cme_error_t err;
  switch (socket->proto_type) {
  case SocketProto_UDP:
    err = cmsu_SocketUdp_send_sync(recver, buffer, socket->proto);
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

static inline cme_error_t
cmsu_Socket_send_async(socket_t socket, ip_addr_t *recver, void *data) {
  cme_error_t err;
  switch (socket->proto_type) {
  case SocketProto_UDP:
    err = cmsu_SocketUdp_send_async(socket->proto, recver, data);
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

#endif // C_MINILIB_SIP_UA_SOCKET_H
