/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_H
#define C_MINILIB_SIP_UA_INT_UDP_H

#include <stdint.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "socket99.h"

#include "utils/error.h"

struct cmsu_SocketUdp {
  int sockfd;
  int recv_timerfd;
  int send_timerfd;

  void *ctx;
  cme_error_t (*recv_calbck)(uint32_t buf_len, char *buf, void *data);
  cme_error_t (*send_calbck)(uint32_t buf_len, char *buf, void *data);
};

// This should take ip and port
static inline cme_error_t cmsu_SocketUdp_create(
    const char *ipaddr, uint32_t port, void *ctx,
    cme_error_t (*recv_calbck)(uint32_t buf_len, char *buf, void *data),
    cme_error_t (*send_calbck)(uint32_t buf_len, char *buf, void *data),
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
  (*sock)->ctx = ctx;
  (*sock)->send_calbck = send_calbck;
  (*sock)->recv_calbck = recv_calbck;

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

static inline cme_error_t cmsu_SocketUdp_recv_callback(void *ctx_) {
  struct cmsu_SocketUdp *ctx = ctx_;
  struct sockaddr sender;
  socklen_t sender_size;
  char buffer[1024];
  cme_error_t err;

  errno = 0;
  memset(buffer, 0, 1024);

  int32_t received_bytes =
      recvfrom(ctx->sockfd, buffer, 1023, MSG_NOSIGNAL, &sender, &sender_size);

  if (received_bytes > 0) {
    printf("Read %d bytes: %s\n", received_bytes, buffer);
  } else if (received_bytes == 0) {
    printf("Connection closed by peer\n");
    return 0;
  } else {
    return cme_return(cme_error(errno, "Error during reciving data over UDP"));
  }

  err = ctx->recv_calbck(1023, buffer, ctx->ctx);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline int cmsu_SocketUdp_get_fd(void *ctx) {
  return ((struct cmsu_SocketUdp *)ctx)->sockfd;
};

static inline void cmsu_SocketUdp_destroy(void *ctx) {
  close(((struct cmsu_SocketUdp *)ctx)->sockfd);
  free(ctx);
};

#endif // C_MINILIB_SIP_UA_SOCKET_H
