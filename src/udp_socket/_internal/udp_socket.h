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
#include "udp_socket/_internal/common.h"
#include "udp_socket/_internal/udp_socket_listen.h"
#include "udp_socket/udp_socket.h"
#include "utils/buffer.h"
#include "utils/ip.h"

static inline cme_error_t __UdpSocket_send(void *data);

static inline cme_error_t __UdpSocket_create(event_loop_t evl, ip_t ip_addr,
                                             udp_socket_t *out) {
  struct __UdpSocketPtr *udp_socketp = malloc(sizeof(struct __UdpSocketPtr));
  cme_error_t err;
  if (!udp_socketp) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `udp_socketp`");
    goto error_out;
  }

  errno = 0;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == -1) {
    err = cme_errorf(errno, "Cannot create udp socket for IP=%s:%s", ip_addr.ip,
                     ip_addr.port);
    goto error_udp_socket_cleanup;
  }

  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
      -1) {
    err = cme_errorf(errno, "Cannot configure udp socket for IP=%s:%s",
                     ip_addr.ip, ip_addr.port);
    goto error_socket_cleanup;
  }

  struct sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(atoi(ip_addr.port));
  int result = inet_pton(AF_INET, ip_addr.ip, &my_addr.sin_addr);
  if (result != 1) {
    err = cme_errorf(errno, "Cannot parse IP=%s:%s", ip_addr.ip, ip_addr.port);
    goto error_socket_cleanup;
  }

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
    err = cme_errorf(errno, "Cannot bind udp socket to IP=%s:%s", ip_addr.ip,
                     ip_addr.port);
    goto error_socket_cleanup;
  }

  err = event_loop_insert_socketfd(evl, sockfd, __UdpSocket_send,
                                   __UdpSocket_recv, udp_socketp);
  if (err) {
    err = cme_errorf(errno,
                     "Cannot insert udp socket into event loop for IP=%s:%s",
                     ip_addr.ip, ip_addr.port);
    goto error_socket_cleanup;
  }

  *udp_socketp = __UdpSocketPtr_from((struct __UdpSocket){
      .evl = event_loop_ref(evl), .ip_addr = ip_addr, .fd = sockfd});

  *out = udp_socketp;

  return 0;

error_socket_cleanup:
  close(sockfd);
error_udp_socket_cleanup:
  free(udp_socketp);
error_out:
  *out = NULL;
  return err;
};

inline static cme_error_t __UdpSocket_send(void *data) {
  struct __UdpSocket *udpsock = data;
  (void)udpsock;
  return 0;
}

static inline udp_socket_t __UdpSocket_ref(udp_socket_t udp_socketp) {

  __UdpSocketPtr_clone(*udp_socketp);

  return udp_socketp;
}

static inline void __UdpSocket_deref(udp_socket_t udp_socketp) {
  int32_t usage_count = __UdpSocketPtr_use_count(udp_socketp);

  __UdpSocketPtr_drop(udp_socketp);

  // If usage count is 1 before drop it means it will be 0
  //  after drop but ptr holding usage count get freed on drop.
  if (usage_count <= 1) {
    free(udp_socketp);
  }
}

static inline void __UdpSocket_destroy(struct __UdpSocket *udp_socket) {
  // Remove fd from event loop
  event_loop_remove_fd(udp_socket->evl, udp_socket->fd);
  event_loop_deref(udp_socket->evl);

  // Close socket file descriptor
  close(udp_socket->fd);
};

static inline struct __UdpSocket
__UdpSocket_clone(struct __UdpSocket udp_socket) {
  return udp_socket;
};

#endif
