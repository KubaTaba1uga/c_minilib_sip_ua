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
#include "udp/udp.h"
#include "utils/ip.h"

#ifndef CMSU_UDP_MSG_SIZE_MAX
#define CMSU_UDP_MSG_SIZE_MAX 2048
#endif

struct cmsu_UdpSocket {
  // Event loop data
  event_loop_t evl;
  int32_t fd;

  // Udp local data
  ip_t ip_addr;

  // User data & ops
  udp_recvh_t recvh;
  void *recvh_arg;
};

static inline cme_error_t cmsu_UdpSocket_recv(void *data);
static inline cme_error_t cmsu_UdpSocket_send(void *data);

static inline cme_error_t cmsu_UdpSocket_create(event_loop_t evl, ip_t ip_addr,
                                                udp_socket_t *out) {
  struct cmsu_UdpSocket *udpsock = calloc(1, sizeof(struct cmsu_UdpSocket));
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

  err = event_loop_insert_socketfd(evl, (fd_t){.fd = sockfd, .events = 0},
                                   cmsu_UdpSocket_send, cmsu_UdpSocket_recv,
                                   udpsock);
  if (err) {
    err = cme_errorf(errno,
                     "Cannot insert udp socket into event loop for IP=%s:%s",
                     ip_addr.ip, ip_addr.port);
    goto error_socket_cleanup;
  }

  udpsock->evl = evl;
  udpsock->ip_addr = ip_addr;
  udpsock->fd = sockfd;

  *out = udpsock;

  return 0;

error_socket_cleanup:
  close(sockfd);
error_udpsock_cleanup:
  free(udpsock);
error_out:
  *out = NULL;
  return err;
};

static inline cme_error_t cmsu_UdpSocket_recv(void *data) {
  struct cmsu_UdpSocket *udpsock = data;
  struct sockaddr_storage sender_addr;
  socklen_t sender_addr_len;
  int32_t buffer_len;
  cme_error_t err;
  char *buffer;

  puts("Received data over UDP");

  assert(data != NULL);

  sender_addr_len = sizeof(sender_addr);

  buffer = calloc(CMSU_UDP_MSG_SIZE_MAX, sizeof(char));
  if (!buffer) {
    err = cme_error(errno, "Cannot allocate memory for `buffer`");
    goto error_out;
  }

  errno = 0;
  buffer_len =
      recvfrom(udpsock->fd, buffer, CMSU_UDP_MSG_SIZE_MAX - 1, MSG_NOSIGNAL,
               (struct sockaddr *)&sender_addr, &sender_addr_len);

  if (buffer_len < 0) {
    err = cme_error(errno, "Cannot recieve udp data");
    goto error_out;
  } else if (buffer_len == 0) {
    err = cme_error(errno, "Connection closed by peer");
    goto error_out;
  }

  if (udpsock->recvh) {
    char ip_str[INET6_ADDRSTRLEN];
    char port_str[6];

    struct sockaddr_in *s = (struct sockaddr_in *)&sender_addr;
    inet_ntop(AF_INET, &s->sin_addr, ip_str, sizeof(ip_str));
    snprintf(port_str, sizeof(port_str), "%u", ntohs(s->sin_port));

    err = udpsock->recvh(buffer, buffer_len,
                         (ip_t){.ip = ip_str, .port = port_str}, udpsock,
                         udpsock->recvh_arg);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t cmsu_UdpSocket_send(void *data) {
  struct cmsu_UdpSocket *udpsock = data;
  (void)udpsock;
  return 0;
}

static inline void cmsu_UdpSocket_destroy(udp_socket_t *out) {
  if (!out || !*out) {
    return;
  }

  event_loop_remove_fd((*out)->evl, (*out)->fd);
  free(*out);
  *out = NULL;
}

#endif // C_MINILIB_SIP_UA_INT_UDP_H
