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
#include "stc/common.h"
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

static inline struct __UdpSocket
__UdpSocket_clone(struct __UdpSocket udp_socket) {
  puts(__func__);
  return udp_socket;
};

static inline cme_error_t __UdpSocket_create_fd(struct IpAddrPtr ip_addr,
                                                int32_t *fd) {
  errno = 0;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  cme_error_t err;

  if (sockfd == -1) {
    err = cme_errorf(errno, "Cannot create udp socket for IP=%s:%s",
                     ip_addr.get->ip, ip_addr.get->port);
    goto error_out;
  }

  int optval = 1;
  errno = 0;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
      -1) {
    err = cme_errorf(errno, "Cannot configure udp socket for IP=%s:%s",
                     ip_addr.get->ip, ip_addr.get->port);
    goto error_out;
  }

  struct sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  errno = 0;
  my_addr.sin_port = htons(atoi(cstr_data(&ip_addr.get->port)));
  if (inet_pton(AF_INET, cstr_data(&ip_addr.get->ip), &my_addr.sin_addr) != 1) {
    err = cme_errorf(errno, "Cannot parse IP=%s:%s", ip_addr.get->ip,
                     ip_addr.get->port);
    goto error_out;
  }

  errno = 0;
  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
    err = cme_errorf(errno, "Cannot bind udp socket to IP=%s:%s",
                     ip_addr.get->ip, ip_addr.get->port);
    goto error_out;
  }

  *fd = sockfd;

  return 0;

error_out:
  close(sockfd);
  return cme_return(err);
}

#endif
