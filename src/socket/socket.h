/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SOCKET_H
#define C_MINILIB_SIP_UA_SOCKET_H
/*
  This is interface to socket module, if you need anything from socket module,
   put interface declaration here and interface implementation in .c but always
   write real implementation as static inline in _internal.
 */

#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "utils/common.h"

/******************************************************************************
 *                             Socket                                         *
 ******************************************************************************/
typedef struct {
  const char *ip;
  const char *port;
} ip_addr_t;

typedef struct cmsu_Socket *socket_t;

enum SocketType {
  SocketType_NONE = 0,
  SocketType_UDP,
  SocketType_MAX,
};

cme_error_t socket_udp_create(
    cmsu_evl_t evl, ip_addr_t ipaddr,
    cme_error_t (*recv_callback)(socket_t sock, buffer_t *buf, void *ctx),
    cme_error_t (*send_callback)(socket_t sock, ip_addr_t recver, buffer_t *buf,
                                 void *ctx),
    void(*ctx_destroy), void *ctx, socket_t *out);
cme_error_t socket_recv(socket_t socket);
cme_error_t socket_send(ip_addr_t recver, buffer_t buf, socket_t socket);

/******************************************************************************
 *                             Sockets Vector                                 *
 ******************************************************************************/
typedef struct vec_cmsu_Sockets *vec_socket_t;
cme_error_t vec_socket_create(vec_socket_t *sockets);
socket_t vec_socket_get(uint32_t fd, vec_socket_t sockets);
cme_error_t vec_socket_insert(socket_t socket, vec_socket_t sockets);
void vec_socket_destroy(vec_socket_t *sockets);

#endif // C_MINILIB_SIP_UA_SOCKET_H
