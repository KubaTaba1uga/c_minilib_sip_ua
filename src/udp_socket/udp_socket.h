/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UDP_SOCKET_H
#define C_MINILIB_SIP_UA_UDP_SOCKET_H
/*
  This is interface to udp_socket module, if you need anything from udp_socket
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */

#include <poll.h>
#include <stdint.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "utils/ip.h"
#include "utils/socket.h"

// Helper to prettify this header
#include "udp_socket/udp_socket_ptr.h"

/******************************************************************************
 *                             Udp Socket                                     *
 ******************************************************************************/
typedef struct __UdpSocketPtr udp_socket_t;

cme_error_t udp_socket_create(event_loop_t evl, ip_t ip_addr,
                              udp_socket_t *out);
udp_socket_t udp_socket_ref(udp_socket_t evl);
void udp_socket_deref(udp_socket_t *evl);
cme_error_t udp_socket_listen(udp_socket_t udp_socket, socket_recvh_t recvh,
                              void *arg);

#endif // C_MINILIB_SIP_UA_UDP_SOCKET_H
