/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_LISTEN_H
#define C_MINILIB_SIP_UA_INT_UDP_LISTEN_H
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "udp/_internal/udp.h"
#include "udp/udp.h"
#include "utils/ip.h"

cme_error_t cmsu_UdpSocket_listen(struct cmsu_UdpSocket *udp_socket,
                                  udp_recvh_t recvh, void *arg) {
  cme_error_t err = event_loop_fd_set_pollin(udp_socket->evl, udp_socket->fd);
  if (err) {
    goto error_out;
  }

  udp_socket->recvh = recvh;
  udp_socket->recvh_arg = arg;

  return 0;

error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_INT_UDP_H
