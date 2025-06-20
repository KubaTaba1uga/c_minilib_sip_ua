/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_H
#define C_MINILIB_SIP_UA_INT_UDP_H
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
#include "stc/cstr.h"
#include "utils/buffer.h"
#include "utils/csview_ptr.h"
#include "utils/ip.h"

#ifndef __UDP_MSG_SIZE_MAX
#define __UDP_MSG_SIZE_MAX 2048
#endif

typedef cme_error_t (*udp_socket_recvh_t)(csview_ptr_t buf, ip_t peer,
                                          void *data);

struct __UdpSocket {
  // Event loop data
  struct EventLoopPtr evl;
  int32_t fd;

  // Udp local data
  ip_t ip_addr;

  // User data & ops
  udp_socket_recvh_t recvh;
  void *recvh_arg;
};

static inline void __UdpSocket_destroy(struct __UdpSocket *udp_socket) {
  // Remove fd from event loop
  EventLoopPtr_remove_fd(udp_socket->evl, udp_socket->fd);
  EventLoopPtr_drop(&udp_socket->evl);

  // Close socket file descriptor
  close(udp_socket->fd);
};

static inline struct __UdpSocket
__UdpSocket_clone(struct __UdpSocket udp_socket) {
  return udp_socket;
};

#endif
