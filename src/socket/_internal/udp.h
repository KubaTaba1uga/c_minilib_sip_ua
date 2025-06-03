/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_H
#define C_MINILIB_SIP_UA_INT_UDP_H
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "c_minilib_mock.h"
#include "event_loop/event_loop.h"
#include "socket/_internal/common.h"
#include "socket/_internal/request.h"
#include "socket/_internal/request_queue.h"
#include "socket/socket.h"
#include "socket99.h"
#include "stc/common.h"
#include "utils/common.h"

/******************************************************************************
 *                                Udp                                      *
 ******************************************************************************/
#ifndef CMSU_UDP_MSG_SIZE
#define CMSU_UDP_MSG_SIZE 1536
#endif

struct cmsu_SocketUdp {
  queue_cmsu_Requests reqs_queue; // We need to be able to store requests in
                                  // case user schedule multiple sends.
  struct cmsu_Socket socket;
  ip_addr_t ipaddr;
  int sockfd;
  evl_t evl;

  // Logic callback
  void *ctx;
  cme_error_t (*recv_callback)(socket_t socket, ip_addr_t *sender,
                               buffer_t *buffer, void *proto);
  cme_error_t (*recv_fail_callback)(socket_t socket, ip_addr_t *sender,
                                    buffer_t *buffer, void *proto);
  cme_error_t (*send_callback)(socket_t socket, ip_addr_t *recver,
                               buffer_t *buffer, void *data, void *proto);
  cme_error_t (*send_fail_callback)(socket_t socket, ip_addr_t *recver,
                                    buffer_t *buffer, void *data, void *proto);
  void (*destroy_callback)(void *proto);
};

MOCKABLE_STATIC(int cmsu_SocketUdp_recvfrom(int sockfd, void *buf, size_t len,
                                            int flags,
                                            struct sockaddr *src_addr,
                                            socklen_t *addrlen)) {
  return recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

MOCKABLE_STATIC(int cmsu_SocketUdp_sendto(int sockfd, const void *buf,
                                          size_t len, int flags,
                                          const struct sockaddr *dest_addr,
                                          socklen_t addrlen)) {
  return sendto(sockfd, buf, len, flags, dest_addr, addrlen);
}

static inline cme_error_t cmsu_SocketUdp_create(
    evl_t evl, ip_addr_t ipaddr, uint32_t socketev, void *ctx,
    cme_error_t (*recv_callback)(socket_t socket, ip_addr_t *sender,
                                 buffer_t *buf, void *ctx_),
    cme_error_t (*recv_fail_callback)(socket_t socket, ip_addr_t *sender,
                                      buffer_t *buf, void *ctx_),
    cme_error_t (*send_callback)(socket_t socket, ip_addr_t *recver,
                                 buffer_t *buf, void *data, void *ctx_),
    cme_error_t (*send_fail_callback)(socket_t socket, ip_addr_t *recver,
                                      buffer_t *buf, void *data, void *ctx_),
    void(*ctx_destroy), socket_t *out) {
  struct cmsu_SocketUdp *udp = calloc(1, sizeof(struct cmsu_SocketUdp));
  uint32_t events = 0;
  cme_error_t err;
  if (!udp) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `udp`");
    goto error_out;
  }

  udp->socket.evl = evl;
  udp->ipaddr = ipaddr;
  udp->socket.proto_type = SocketProto_UDP;
  udp->socket.proto = udp;
  udp->ctx = ctx;
  udp->destroy_callback = ctx_destroy;
  if (socketev & SocketEvent_RECEIVE) {
    events |= POLLIN;
  }
  if (socketev & SocketEvent_SEND) {
    events |= POLLOUT;
  }

  if (recv_callback) {
    udp->recv_callback = recv_callback;
  }
  if (recv_fail_callback) {
    udp->recv_fail_callback = recv_fail_callback;
  }
  if (udp->send_callback) {
    udp->send_callback = send_callback;
  }
  if (send_fail_callback) {
    udp->send_fail_callback = send_fail_callback;
  }

  udp->reqs_queue = queue_cmsu_Requests_init();

  int v_true = 1;
  socket99_result res;
  bool ok = socket99_open(
      &(socket99_config){
          .host = (char *)ipaddr.ip,
          .port = atoi(ipaddr.port),
          .server = recv_callback != NULL,
          .datagram = true,
          .nonblocking = true,
          .sockopts =
              {
                  {SO_REUSEADDR, &v_true, sizeof(v_true)},
              },
      },
      &res);
  if (!ok) {
    err = cme_error(res.saved_errno, "Cannot create socket");
    goto error_udp_cleanup;
  }

  udp->sockfd = res.fd;

  err = event_loop_insert_socket(&udp->socket, events, udp->socket.evl);
  if (err) {
    goto error_udp_cleanup;
  }

  *out = &udp->socket;

  return 0;

error_udp_cleanup:
  free(udp);
error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsu_SocketUdp_recv_sync(ip_addr_t *sender, buffer_t *buffer,
                         struct cmsu_SocketUdp *sock) {
  struct sockaddr tmp_sendr;
  socklen_t tmp_sendr_size;
  cme_error_t err;

  buffer->buf = calloc(CMSU_UDP_MSG_SIZE, sizeof(char));
  if (!buffer->buf) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `buffer->buf`");
    goto error_out;
  }

  buffer->len =
      cmsu_SocketUdp_recvfrom(sock->sockfd, buffer->buf, CMSU_UDP_MSG_SIZE,
                              MSG_NOSIGNAL, &tmp_sendr, &tmp_sendr_size);

  if (buffer->len > 0) {
    printf("Read %d bytes: %.*s\n", buffer->len, buffer->len, buffer->buf);
  } else if (buffer->len == 0) {
    printf("Connection closed by peer\n");
    goto out;
  } else {
    err = cme_error(errno, "Error during reciving data over UDP");
    goto error_out;
  }

  char ipstr[INET6_ADDRSTRLEN];
  char portstr[6]; // Max 65535 + null

  getnameinfo(&tmp_sendr, tmp_sendr_size, ipstr, sizeof(ipstr), portstr,
              sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);

  sender->ip = calloc(strlen(ipstr), sizeof(char));
  if (!sender->ip) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `sender->ip`");
    goto error_out;
  }

  strncpy((char *)sender->ip, ipstr, strlen(ipstr));

  sender->port = calloc(strlen(portstr), sizeof(char));
  if (!sender->port) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `sender->port`");
    goto error_out;
  }

  strncpy((char *)sender->port, portstr, strlen(portstr));

out:
  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsu_SocketUdp_recv_event_handler(struct cmsu_SocketUdp *sock) {
  ip_addr_t sender;
  buffer_t buffer;
  cme_error_t err;

  err = cmsu_SocketUdp_recv_sync(&sender, &buffer, sock);
  if (err) {
    if (sock->recv_fail_callback) {
      sock->recv_fail_callback(&sock->socket, &sender, &buffer, sock->ctx);
    }
    goto error_out;
  }

  if (sock->recv_callback) {
    err = sock->recv_callback(&sock->socket, &sender, &buffer, sock->ctx);
    if (err) {
      goto error_out;
    }
    return 0;
  }

  return 0;

error_out:
  event_loop_remove_socket(&sock->socket);
  return cme_return(err);
}

cme_error_t cmsu_SocketUdp_send_sync(ip_addr_t *recver, buffer_t *buffer,
                                     struct cmsu_SocketUdp *sock) {
  struct addrinfo *receiver_addr = NULL;
  struct addrinfo hints = {0};
  cme_error_t err;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  if (buffer->len > CMSU_UDP_MSG_SIZE) {
    err = cme_error(ENOBUFS, "Too much data to send over UDP");
    goto error_out;
  }

  {
    int err_ = getaddrinfo(recver->ip, recver->port, &hints, &receiver_addr);
    if (err_) {
      err = cme_error(err_, "Cannot get address info");
      goto error_out;
    }
  }

  int32_t sent_bytes = cmsu_SocketUdp_sendto(
      sock->sockfd, buffer->buf, buffer->len, MSG_NOSIGNAL,
      receiver_addr->ai_addr, receiver_addr->ai_addrlen);
  if (sent_bytes > 0) {
    printf("Write %d bytes: %.*s\n", sent_bytes, sent_bytes, buffer->buf);
  } else if (sent_bytes == 0) {
    printf("Connection closed by peer\n");
    goto out;
  } else {
    err = cme_error(errno, "Error during reciving data over UDP");
    goto error_out;
  }

out:
  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsu_SocketUdp_send_event_handler(struct cmsu_SocketUdp *sock,
                                  bool *is_send_done) {
  cme_error_t err;

  struct cmsu_Request request = queue_cmsu_Requests_pull(&sock->reqs_queue);

  if (sock->send_callback) {
    err = sock->send_callback(&sock->socket, &request.recver, &request.buffer,
                              request.data, sock->ctx);
    if (err) {
      goto error_out;
    }
  }

  err = cmsu_SocketUdp_send_sync(&request.recver, &request.buffer,
                                 request.socket->proto);
  if (err) {
    if (sock->send_fail_callback) {
      sock->send_fail_callback(&sock->socket, &request.recver, &request.buffer,
                               request.data, sock->ctx);
    }
    goto error_out;
  }

  *is_send_done = queue_cmsu_Requests_size(&sock->reqs_queue) > 0;
  free(request.buffer.buf);

  return 0;

error_out:
  event_loop_remove_socket(&sock->socket);
  return cme_return(err);
};

static inline int cmsu_SocketUdp_get_fd(struct cmsu_SocketUdp *sock) {
  return sock->sockfd;
};

static inline void cmsu_SocketUdp_destroy(struct cmsu_SocketUdp *sock) {
  if (!sock) {
    return;
  }

  free(sock);
};

static inline cme_error_t
cmsu_SocketUdp_send_async(struct cmsu_SocketUdp *socket_udp, ip_addr_t *recver,
                          void *data) {
  cme_error_t err;

  struct cmsu_Request *request = queue_cmsu_Requests_push(
      &socket_udp->reqs_queue,
      (struct cmsu_Request){.socket = &socket_udp->socket,
                            .data = data,
                            .recver = (ip_addr_t){
                                .ip = strdup(recver->ip),
                                .port = strdup(recver->port),
                            }});
  if (!request) {
    err = cme_error(ENOMEM, "Cannot insert new request into UDP socket");
    goto error_out;
  }

  event_loop_async_send_socket(&socket_udp->socket);

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_UDP_H
