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

// Socket protos
typedef struct cmsu_Socket *cmsu_sock_t;
typedef struct cmsu_SocketUdp *cmsu_sock_udp_t;

// Socket datastructures
typedef struct list_cmsu_Sockets *cmsu_sock_list_t;

// Ops
void cmsu_sock_destroy(cmsu_sock_t *socket);

int cmsu_sock_get_fd(cmsu_sock_t socket);

cme_error_t cmsu_sock_recv(cmsu_sock_t socket);

cme_error_t cmsu_sock_send(cmsu_sock_t socket);

cme_error_t cmsu_sock_list_create(cmsu_sock_list_t *sockets);

struct cmsu_SocketArg {
  void *ctx;
  cme_error_t (*recvh)(uint32_t buf_len, char *buf, void *ctx);
  cme_error_t (*sendh)(uint32_t buf_len, char *buf, void *ctx);
  void (*destroyh)(void *ctx);
};
cme_error_t cmsu_sock_list_insert_udp(const char *ipaddr, uint32_t port,
                                      struct cmsu_SocketArg sockarg,
                                      cmsu_sock_t *out,
                                      cmsu_sock_list_t sockets);

cmsu_sock_t cmsu_sock_list_find(int fd, cmsu_sock_list_t sockets);

void cmsu_sock_list_destroy(cmsu_sock_list_t *sockets);

#endif // C_MINILIB_SIP_UA_SOCKET_H
