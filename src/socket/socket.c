/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <errno.h>

#include "c_minilib_error.h"
#include "socket/_internal/socket.h"
#include "socket/_internal/socket_list.h"
#include "socket/_internal/udp.h"
#include "socket/socket.h"
#include "stc/common.h"

int cmsu_sock_get_fd(cmsu_sock_t socket) {
  if (!socket || !socket->ctx) {
    return -1;
  }

  return socket->get_fd_func(socket->ctx);
}

cme_error_t cmsu_sock_recv(cmsu_sock_t socket) {
  cme_error_t err;
  if (!socket || !socket->ctx) {
    err = cme_error(EINVAL, "`socket` and `socket->ctx` cannot be NULL");
    goto error_out;
  }

  err = socket->recv_clbck(socket->ctx);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsu_sock_send(cmsu_sock_t socket) {
  cme_error_t err;
  if (!socket || !socket->ctx) {
    err = cme_error(EINVAL, "`socket` and `socket->ctx` cannot be NULL");
    goto error_out;
  }

  err = socket->send_clbck(socket->ctx);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

void cmsu_sock_destroy(cmsu_sock_t *socket) {
  if (!socket || !*socket) {
    return;
  }

  int fd = cmsu_sock_get_fd(*socket);
  if (fd > 0) {
    close(fd);
  }

  free((*socket)->ctx);
  free(socket);
}

cme_error_t cmsu_sock_list_create(cmsu_sock_list_t *sockets) {
  struct list_cmsu_Sockets *list = malloc(sizeof(struct list_cmsu_Sockets));
  if (!list) {
    return cme_return(cme_error(ENOBUFS, "Cannot allocate `socket`"));
  }

  *list = list_cmsu_Sockets_init();

  *sockets = list;

  return 0;
}

void cmsu_sock_list_destroy(cmsu_sock_list_t *sockets) {
  if (!sockets || !*sockets) {
    return;
  }

  free(*sockets);
  *sockets = NULL;
}

cme_error_t cmsu_sock_list_insert_udp(
    const char *ipaddr, uint32_t port, void *ctx,
    cme_error_t (*recv_calbck)(uint32_t buf_len, char *buf, void *ctx),
    cme_error_t (*send_calbck)(uint32_t buf_len, char *buf, void *ctx),
    cmsu_sock_t *out, cmsu_sock_list_t sockets) {
  struct cmsu_SocketUdp *udp_sock;
  cme_error_t err;

  err = cmsu_SocketUdp_create(ipaddr, port, ctx, recv_calbck, send_calbck,
                              &udp_sock);
  if (err) {
    goto error_out;
  }

  cmsu_sock_t result = list_cmsu_Sockets_push(
      sockets, (struct cmsu_Socket){
                   .ctx = udp_sock,
                   .send_clbck = NULL,
                   .proto = cmsu_SupportedSockets_UDP,
                   .get_fd_func = cmsu_SocketUdp_get_fd,
                   .recv_clbck = cmsu_SocketUdp_recv_callback,
               });
  if (!result) {
    err = cme_error(ENOBUFS, "Cannot push to `sockets`");
    goto error_udp_cleanup;
  }

  *out = result;

  return 0;
error_udp_cleanup:
  cmsu_SocketUdp_destroy(udp_sock);
error_out:
  return cme_return(err);
}

cmsu_sock_t cmsu_sock_list_find(int fd, cmsu_sock_list_t sockets) {
  c_foreach(item, list_cmsu_Sockets, *sockets) {
    if (fd == item.ref->get_fd_func(item.ref->ctx)) {
      return item.ref;
    }
  }
  return NULL;
}
