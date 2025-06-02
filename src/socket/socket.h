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

enum SocketProto {
  SocketProto_NONE = 0,
  SocketProto_UDP,
  SocketProto_MAX,
};

enum SocketEvent {
  SocketEvent_SEND = 1,
  SocketEvent_RECEIVE = 2,
};

cme_error_t socket_udp_create(
    evl_t evl, ip_addr_t ipaddr, uint32_t socketev, void *ctx,
    cme_error_t (*recv_callback)(socket_t socket, ip_addr_t *sender,
                                 buffer_t *buf, void *ctx_),
    cme_error_t (*recv_fail_callback)(socket_t socket, ip_addr_t *sender,
                                      buffer_t *buf, void *ctx_),
    cme_error_t (*send_callback)(socket_t socket, ip_addr_t *recver,
                                 buffer_t *buf, void *data, void *ctx_),
    cme_error_t (*send_fail_callback)(socket_t socket, ip_addr_t *recver,
                                      buffer_t *buf, void *data, void *ctx_),
    void(*ctx_destroy), socket_t *out);
cme_error_t socket_recv_event_handler(socket_t socket);
cme_error_t socket_send_event_handler(socket_t socket, bool *is_send_done);
cme_error_t socket_fail_event_handler(bool is_err, bool is_conn_close,
                                      socket_t socket);

evl_t socket_get_evl(socket_t socket);
int socket_get_fd(socket_t socket);
void socket_destroy(socket_t *socket);

cme_error_t socket_recv_sync(ip_addr_t *sender, buffer_t *buffer,
                             socket_t socket);
cme_error_t socket_send_sync(ip_addr_t *recver, buffer_t *buffer,
                             socket_t socket);

cme_error_t socket_send_async(socket_t socket, ip_addr_t *recver, void *data);

/******************************************************************************
 *                             Sockets Vector                                 *
 ******************************************************************************/
typedef struct vec_cmsu_Sockets *vec_socket_t;
cme_error_t vec_socket_create(vec_socket_t *sockets);
socket_t vec_socket_get(uint32_t fd, vec_socket_t sockets);
cme_error_t vec_socket_insert(socket_t socket, vec_socket_t sockets);
void vec_socket_destroy(vec_socket_t *sockets);
void vec_socket_remove(uint32_t fd, vec_socket_t sockets);

#endif // C_MINILIB_SIP_UA_SOCKET_H
