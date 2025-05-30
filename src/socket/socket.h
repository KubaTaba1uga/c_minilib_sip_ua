/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SOCKET_H
#define C_MINILIB_SIP_UA_SOCKET_H
/*
  This is interface to socket module, if you need anything from socket module,
   put here and in .c interface and write implementation as static inline in
   _internal.
 */

#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "utils/error.h"

typedef struct cmsu_Socket *cmsu_sock_t;
typedef struct list_cmsu_Sockets *cmsu_sock_list_t;

typedef struct {
  const char *ipaddr;
  uint32_t port;
} cmsu_sock_ip_addr;

typedef struct {
  cmsu_sock_ip_addr addr;
  void *ctx;
  void (*ctx_destroyh)(void *ctx);
} cmsu_sock_arg;
cme_error_t cmsu_sock_list_insert_udp(cmsu_sock_arg args, cmsu_sock_t *out,
                                      cmsu_sock_list_t sockets);
typedef struct {
  cme_error_t (*recv_callback)(cmsu_sock_ip_addr raddr, uint32_t buf_len,
                               const char *buf, void *ctx);
} cmsu_sock_listen_arg;
cme_error_t cmsu_sock_listen(cmsu_sock_listen_arg args, cmsu_sock_t socket);

typedef struct cmsu_sock_send_arg_t {
  cmsu_sock_ip_addr addr;
  uint32_t buf_len;
  const char *buf;
} cmsu_sock_send_arg;
cme_error_t cmsu_sock_send(cmsu_sock_send_arg args, cmsu_sock_t socket);

cme_error_t cmsu_sock_list_create(cmsu_sock_list_t *sockets);

cmsu_sock_t cmsu_sock_list_find(int fd, cmsu_sock_list_t sockets);

void cmsu_sock_list_destroy(cmsu_sock_list_t *sockets);

#endif // C_MINILIB_SIP_UA_SOCKET_H
