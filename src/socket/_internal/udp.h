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
#include "socket/_internal/socket.h"
#include "socket/socket.h"
#include "socket99.h"
#include "utils/common.h"

/******************************************************************************
 *                                Udp                                      *
 ******************************************************************************/
struct cmsu_SocketUdp {
  void *ctx;
  cme_error_t (*recv_callback)(cmsu_evl_t evl, ip_addr_t sender, buffer_t buf,
                               void *ctx);
  cme_error_t (*send_callback)(cmsu_evl_t evl, ip_addr_t recver, buffer_t buf,
                               void *ctx);
};

static inline cme_error_t cmsu_SocketUdp_recv(cmsu_evl_t evl, void *ctx);
static inline cme_error_t cmsu_SocketUdp_send(cmsu_evl_t evl, void *ctx);

static inline cme_error_t cmsu_SocketUdp_create(socket_udp_create_arg args,
                                                socket_t *out) {
  struct cmsu_SocketUdp *udp = calloc(1, sizeof(struct cmsu_SocketUdp));
  cme_error_t err;
  if (!udp) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `udp`");
    goto error_out;
  }

  udp->ctx = args.ctx;
  udp->recv_callback = args.recv_callback;
  udp->send_callback = args.send_callback;

  int v_true = 1;
  socket99_result res;
  bool ok = socket99_open(
      &(socket99_config){
          .host = (char *)args.ipaddr.ip,
          .port = atoi(args.ipaddr.port),
          .server = args.recv_callback != NULL,
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

  err = cmsu_Socket_create(
      (struct cmsu_Socket){
          .type = cmsu_SupportedSocket_UDP,
          .ctx_destroy = args.ctx_destroy,
          .recv = cmsu_SocketUdp_recv,
          .send = cmsu_SocketUdp_send,
          .ipaddr = args.ipaddr,
          .evl = args.evl,
          .sockfd = res.fd,
          .ctx = udp,
      },
      out);
  if (err) {
    goto error_udp_cleanup;
  }
  return 0;

error_udp_cleanup:
  free(udp);
error_out:
  return cme_return(err);
};

static inline cme_error_t cmsu_SocketUdp_recv(cmsu_evl_t evl, void *ctx) {
  return 0;
}
static inline cme_error_t cmsu_SocketUdp_send(cmsu_evl_t evl, void *ctx) {
  return 0;
};

#endif // C_MINILIB_SIP_UA_UDP_H
