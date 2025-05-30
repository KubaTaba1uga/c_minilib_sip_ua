/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_H
#define C_MINILIB_SIP_UA_INT_UDP_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "c_minilib_mock.h"
#include "socket/socket.h"
#include "socket99.h"
#include "stc/cstr.h"
#include "stc/csview.h"

#include "utils/error.h"

#ifndef CMSU_UDP_MSG_SIZE
#define CMSU_UDP_MSG_SIZE 1536
#endif

struct cmsu_SocketUdp {
  int sockfd;
  int recv_timerfd;
  int send_timerfd;

  void *ctx;
  cme_error_t (*recvh)(csview buf, void *ctx);
  cme_error_t (*sendh)(cstr buf, void *ctx);
  void (*destroyh)(void *ctx);
};

MOCKABLE_STATIC(int cmsu_SocketUdp_recvfrom(int sockfd, void *buf, size_t len,
                                            int flags,
                                            struct sockaddr *src_addr,
                                            socklen_t *addrlen)) {
  return recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

// This should take ip and port
static inline cme_error_t cmsu_SocketUdp_create(const char *ipaddr,
                                                uint32_t port,
                                                cmsu_sock_arg_t sockarg,
                                                struct cmsu_SocketUdp **sock) {
  cme_error_t err;
  if (!sock) {
    err = cme_error(EINVAL, "`sock` cannot be NULL");
    goto error_out;
  }

  *sock = calloc(1, sizeof(struct cmsu_SocketUdp));
  if (!*sock) {
    err = cme_error(ENOMEM, "Cannot allocate memory for socket udp");
    goto error_out;
  }

  // TO-DO: allow setting up ip and port
  int v_true = 1;
  socket99_result res;
  bool ok = socket99_open(
      &(socket99_config){
          .host = (char *)ipaddr,
          .port = port,
          .server = true,
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
    goto error_sock_cleanup;
  }

  (*sock)->sockfd = res.fd;
  (*sock)->ctx = sockarg.ctx;
  (*sock)->sendh = sockarg.sendh;
  (*sock)->recvh = sockarg.recvh;
  (*sock)->destroyh = sockarg.destroyh;

  return 0;

error_sock_cleanup:
  free(*sock);
  *sock = NULL;
error_out:
  return cme_return(err);
};

static inline void *cmsu_SocketUdp_clone(void *src) {
  struct cmsu_SocketUdp *dst = calloc(1, sizeof(struct cmsu_SocketUdp));
  if (!dst) {
    return NULL;
  }

  *dst = *((struct cmsu_SocketUdp *)src);

  return dst;
}

static inline cme_error_t cmsu_SocketUdp_recv(void *ctx_) {
  struct cmsu_SocketUdp *ctx = ctx_;
  char buf[CMSU_UDP_MSG_SIZE];
  struct sockaddr sender;
  socklen_t sender_size;
  int32_t buf_len;
  cme_error_t err;

  buf_len = cmsu_SocketUdp_recvfrom(ctx->sockfd, buf, CMSU_UDP_MSG_SIZE,
                                    MSG_NOSIGNAL, &sender, &sender_size);
  if (buf_len > 0) {
    printf("Read %d bytes: %.*s\n", buf_len, buf_len, buf);
  } else if (buf_len == 0) {
    printf("Connection closed by peer\n");
    goto out;
  } else {
    err = cme_error(errno, "Error during reciving data over UDP");
    goto error_out;
  }

  csview buf_view = c_sv(buf, buf_len);
  err = ctx->recvh(buf_view, ctx->ctx);
  if (err) {
    goto error_out;
  }

out:
  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t cmsu_SocketUdp_send(cmsu_sock_send_arg_t sarg,
                                              void *ctx_) {
  struct cmsu_SocketUdp *ctx = ctx_;

  cme_error_t err = ctx->sendh(sarg.buf, ctx->ctx);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline int cmsu_SocketUdp_get_fd(void *ctx) {
  return ((struct cmsu_SocketUdp *)ctx)->sockfd;
};

static inline void cmsu_SocketUdp_destroy(void *ctx) {
  struct cmsu_SocketUdp *data = ctx;
  data->destroyh(data->ctx);
  free(ctx);
};

#endif // C_MINILIB_SIP_UA_SOCKET_H
