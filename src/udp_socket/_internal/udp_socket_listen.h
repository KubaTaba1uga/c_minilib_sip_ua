/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_LISTEN_H
#define C_MINILIB_SIP_UA_INT_UDP_LISTEN_H
#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "udp_socket/_internal/udp_socket.h"
#include "udp_socket/udp_socket.h"
#include "utils/ip.h"

static inline cme_error_t __UdpSocket_listen(udp_socket_t udp_socket,
                                             udp_socket_recvh_t recvh,
                                             void *arg) {
  cme_error_t err =
      event_loop_set_pollin(udp_socket->get->evl, udp_socket->get->fd);
  if (err) {
    goto error_out;
  }

  udp_socket->get->recvh = recvh;
  udp_socket->get->recvh_arg = arg;

  return 0;

error_out:
  return err;
};

#endif
