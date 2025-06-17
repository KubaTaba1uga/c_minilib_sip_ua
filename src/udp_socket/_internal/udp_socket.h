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
#include "utils/smart_ptr.h"

#ifndef __UDP_MSG_SIZE_MAX
#define __UDP_MSG_SIZE_MAX 2048
#endif

struct __UdpSocket {
  // Event loop data
  event_loop_ptr_t evl;
  int32_t fd;

  // Udp local data
  ip_t ip_addr;

  // User data & ops
  udp_socket_recvh_t recvh;
  void *recvh_arg;
};

static cme_error_t __UdpSocket_recv(void *data);
static cme_error_t __UdpSocket_send(void *data);

static inline cme_error_t __UdpSocket_create(event_loop_ptr_t evl, ip_t ip_addr,
                                             udp_socket_ptr_t *out) {
  struct __UdpSocket *udpsock = calloc(1, sizeof(struct __UdpSocket));
  cme_error_t err;
  if (!udpsock) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `udpsock`");
    goto error_out;
  }

  errno = 0;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == -1) {
    err = cme_errorf(errno, "Cannot create udp socket for IP=%s:%s", ip_addr.ip,
                     ip_addr.port);
    goto error_udpsock_cleanup;
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

  udpsock->evl = event_loop_ref(evl);
  udpsock->ip_addr = ip_addr;
  udpsock->fd = sockfd;

  *out = udp_socket_ptr_from(udpsock);

  err = event_loop_insert_socketfd(evl, sockfd, __UdpSocket_send,
                                   __UdpSocket_recv, out);
  if (err) {
    err = cme_errorf(errno,
                     "Cannot insert udp socket into event loop for IP=%s:%s",
                     ip_addr.ip, ip_addr.port);
    goto error_socket_ptr_cleanup;
  }

  printf("udpsock=%p, udpsock->fd=%d\n", out, sockfd);

  return 0;

error_socket_ptr_cleanup:
  udp_socket_ptr_drop(out);
error_socket_cleanup:
  close(sockfd);
error_udpsock_cleanup:
  free(udpsock);
error_out:
  *out = udp_socket_ptr_init();
  return err;
};

static inline udp_socket_ptr_t __UdpSocket_ref(udp_socket_ptr_t udp) {
  return udp_socket_ptr_clone(udp);
};

static inline void __UdpSocket_deref(udp_socket_ptr_t *udp) {
  udp_socket_ptr_drop(udp);
};

static cme_error_t __UdpSocket_recv(void *data) {
  udp_socket_ptr_t *udp_ptr = data;
  udp_socket_recvh_t udp_recvh = *SP_GET_PTR(*udp_ptr, recvh);
  void *udp_recvh_arg = SP_GET_PTR(*udp_ptr, recvh_arg);
  int32_t *udp_fd = SP_GET_PTR(*udp_ptr, fd);
  struct sockaddr_storage sender_addr;
  socklen_t sender_addr_len;
  int32_t buf_raw_len;
  cme_error_t err;
  char *buf_raw;

  printf("udpsoc=%p, udpsock->fd=%d\n", udp_ptr, *udp_fd);
  puts("Received data over UDP");

  assert(data != NULL);

  sender_addr_len = sizeof(sender_addr);

  buf_raw = calloc(__UDP_MSG_SIZE_MAX, sizeof(char));
  if (!buf_raw) {
    err = cme_error(errno, "Cannot allocate memory for `buffer`");
    goto error_out;
  }

  errno = 0;
  buf_raw_len = recvfrom(*udp_fd, buf_raw, __UDP_MSG_SIZE_MAX - 1, MSG_NOSIGNAL,
                         (struct sockaddr *)&sender_addr, &sender_addr_len);

  if (buf_raw_len < 0) {
    perror("UDP");
    err = cme_error(errno, "Cannot recieve udp data");
    goto error_out;
  } else if (buf_raw_len == 0) {
    err = cme_error(errno, "Connection closed by peer");
    goto error_out;
  }

  cstr buf = cstr_from_sv((csview){.buf = buf_raw, .size = buf_raw_len});
  buffer_ptr_t buf_ptr = buffer_ptr_from(buf);

  if (udp_recvh) {
    char ip_str[INET6_ADDRSTRLEN];
    char port_str[6];

    struct sockaddr_in *s = (struct sockaddr_in *)&sender_addr;
    inet_ntop(AF_INET, &s->sin_addr, ip_str, sizeof(ip_str));
    snprintf(port_str, sizeof(port_str), "%u", ntohs(s->sin_port));

    err = udp_recvh(buf_ptr, (ip_t){.ip = ip_str, .port = port_str},
                    udp_recvh_arg);
    if (err) {
      goto error_buf_cleanup;
    }
  }

  buffer_ptr_drop(&buf_ptr);

  return 0;

error_buf_cleanup:
  buffer_ptr_drop(&buf_ptr);
error_out:
  return cme_return(err);
}

static cme_error_t __UdpSocket_send(void *data) {
  struct __UdpSocket *udpsock = data;
  (void)udpsock;
  return 0;
}

void __udp_socket_ptr_destroy(__UdpSocketPtr *udp_ptr) {
  if (!udp_ptr) {
    return;
  }

  event_loop_ptr_t evl = (*udp_ptr)->evl;
  int32_t fd = (*udp_ptr)->fd;

  // Remove fd from event loop
  event_loop_remove_fd(evl, fd);

  // Dereference event loop
  event_loop_deref(&evl);

  // Close socket file descriptor
  close(fd);

  // Free __UdpSocket
  free(*udp_ptr);
  *udp_ptr = NULL;
};

__UdpSocketPtr __udp_socket_ptr_clone(__UdpSocketPtr udp) { return udp; };

#endif
