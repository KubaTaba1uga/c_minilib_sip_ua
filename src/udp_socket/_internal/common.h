/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_SOCKET_COMMON_H
#define C_MINILIB_SIP_UA_INT_UDP_SOCKET_COMMON_H
#include <stdint.h>

#include "c_minilib_error.h"
#include "udp_socket/udp_socket.h"

#ifndef __UDP_MSG_SIZE_MAX
#define __UDP_MSG_SIZE_MAX 2048
#endif

struct __UdpSocket {
  // Event loop data
  event_loop_t evl;
  int32_t fd;

  // Udp local data
  ip_t ip_addr;

  // User data & ops
  udp_socket_recvh_t recvh;
  void *recvh_arg;
};

static inline void __UdpSocket_destroy(struct __UdpSocket *udp_socket);
static inline struct __UdpSocket
__UdpSocket_clone(struct __UdpSocket udp_socket);

#define i_type __UdpSocketPtr
#define i_key struct __UdpSocket
#define i_keydrop __UdpSocket_destroy
#define i_keyclone __UdpSocket_clone
#include "stc/arc.h"

#endif // C_MINILIB_SIP_UA_INT_UDP_SOCKET_COMMON_H
