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
#include "udp_socket/udp_socket.h"
#include "utils/buffer.h"
#include "utils/ip.h"

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

static inline cme_error_t __UdpSocket_recv(void *data);
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

  printf("udp_socket=%p, udp_socket->fd=%d\n", udp_socketp,
         udp_socketp->get->fd);

  return 0;

error_socket_cleanup:
  close(sockfd);
error_udp_socket_cleanup:
  free(udp_socketp);
error_out:
  *out = NULL;
  return err;
};

inline static cme_error_t __UdpSocket_recv(void *data) {
  struct __UdpSocketPtr *udp_socket = data;
  struct sockaddr_storage sender_addr;
  socklen_t sender_addr_len;
  int32_t buf_raw_len;
  cstr_view buf_view;
  cme_error_t err;
  cstr buf_raw;

  printf("udpsoc=%p, udp_socket->fd=%d\n", udp_socket, udp_socket->get->fd);

  puts("Received data over UDP");

  assert(data != NULL);

  sender_addr_len = sizeof(sender_addr);

  buf_raw = cstr_with_capacity(__UDP_MSG_SIZE_MAX);
  buf_view = cstr_getview(&buf_raw);

  errno = 0;
  buf_raw_len = recvfrom(udp_socket->get->fd, (void *)buf_view.data,
                         cstr_capacity(&buf_raw) - 1, MSG_NOSIGNAL,
                         (struct sockaddr *)&sender_addr, &sender_addr_len);

  if (buf_raw_len < 0) {
    perror("UDP");
    err = cme_error(errno, "Cannot recieve udp data");
    goto error_buf_cleanup;
  } else if (buf_raw_len == 0) {
    err = cme_error(errno, "Connection closed by peer");
    goto error_buf_cleanup;
  }

  cstr_resize(&buf_raw, buf_raw_len, 0);

  if (udp_socket->get->recvh) {
    char ip_str[INET6_ADDRSTRLEN];
    char port_str[6];

    struct sockaddr_in *s = (struct sockaddr_in *)&sender_addr;
    inet_ntop(AF_INET, &s->sin_addr, ip_str, sizeof(ip_str));
    snprintf(port_str, sizeof(port_str), "%u", ntohs(s->sin_port));

    err =
        udp_socket->get->recvh(buf_raw, (ip_t){.ip = ip_str, .port = port_str},
                               udp_socket->get->recvh_arg);
    if (err) {
      goto error_buf_cleanup;
    }
  }

  cstr_drop(&buf_raw);

  return 0;

error_buf_cleanup:
  cstr_drop(&buf_raw);
  return cme_return(err);
}

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
