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
#include "stc/cstr.h"

#include "event_loop/event_loop.h"
#include "utils/buffer.h"
#include "utils/buffer_ptr.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"

#ifndef __UDP_MSG_SIZE_MAX
#define __UDP_MSG_SIZE_MAX 2048
#endif

typedef cme_error_t (*udp_socket_recvh_t)(struct BufferPtr buf,
                                          struct IpAddrPtr peer,
                                          struct GenericPtr data);

struct __UdpSocket {
  // Event loop data
  struct EventLoopPtr evl;
  int32_t fd;

  // Udp local data
  struct IpAddrPtr ip_addr;

  // User data & ops
  udp_socket_recvh_t recvh;
  struct GenericPtr recvh_arg;
};

void __UdpSocket_destroy(struct __UdpSocket *udp_socket);

struct __UdpSocket __UdpSocket_clone(struct __UdpSocket udp_socket);

#endif
