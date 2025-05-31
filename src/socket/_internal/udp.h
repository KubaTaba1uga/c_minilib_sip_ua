/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_H
#define C_MINILIB_SIP_UA_INT_UDP_H
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "socket/_internal/common.h"
#include "socket/socket.h"
#include "socket99.h"
#include "utils/common.h"

/******************************************************************************
 *                                Udp                                      *
 ******************************************************************************/
struct cmsu_SocketUdp {
  struct cmsu_Socket socket;
  ip_addr_t ipaddr;
  cmsu_evl_t evl;
  int sockfd;
  void *ctx;

  cme_error_t (*recv_callback)(socket_t sock, buffer_t *buf, void *ctx);
  cme_error_t (*send_callback)(socket_t sock, ip_addr_t recver, buffer_t *buf,
                               void *ctx);
  void (*ctx_destroy)(void *ctx);
};

static inline cme_error_t cmsu_SocketUdp_recv(struct cmsu_SocketUdp *sock);
static inline cme_error_t cmsu_SocketUdp_send(struct cmsu_SocketUdp *sock,
                                              ip_addr_t recver, buffer_t *buf);

static inline cme_error_t cmsu_SocketUdp_create(
    cmsu_evl_t evl, ip_addr_t ipaddr, void *ctx,
    cme_error_t (*recv_callback)(socket_t sock, buffer_t *buf, void *ctx),
    cme_error_t (*send_callback)(socket_t sock, ip_addr_t recver, buffer_t *buf,
                                 void *ctx),
    void(*ctx_destroy), socket_t *out) {
  struct cmsu_SocketUdp *udp = calloc(1, sizeof(struct cmsu_SocketUdp));
  cme_error_t err;
  if (!udp) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `udp`");
    goto error_out;
  }

  udp->evl = evl;
  udp->ipaddr = ipaddr;
  udp->socket.type = SocketType_UDP;
  udp->socket.proto = udp;
  udp->ctx = ctx;
  udp->ctx_destroy = ctx_destroy;
  if (recv_callback) {
    udp->recv_callback = recv_callback;
  }
  if (udp->send_callback) {
    udp->send_callback = send_callback;
  }

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

  *out = &udp->socket;

  return 0;

error_udp_cleanup:
  free(udp);
error_out:
  return cme_return(err);
};

static inline cme_error_t cmsu_SocketUdp_recv(struct cmsu_SocketUdp *sock) {
  puts("Recv HIT");
  return 0;
}
static inline cme_error_t cmsu_SocketUdp_send(struct cmsu_SocketUdp *sock,
                                              ip_addr_t recver, buffer_t *buf) {
  puts("Send HIT");
  return 0;
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

#endif // C_MINILIB_SIP_UA_UDP_H
