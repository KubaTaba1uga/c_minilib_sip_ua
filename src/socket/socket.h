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

/* The only direct user of socket api is event loop. If you need to use socket
 * look into event loop api. */

/******************************************************************************
 *                             Socket                                         *
 ******************************************************************************/
typedef struct {
  const char *ip;
  const char *port;
} ip_addr_t;

typedef struct cmsu_Socket *socket_t;

typedef struct {
  // Data
  ip_addr_t ipaddr;
  cmsu_evl_t evl;
  void *ctx;

  // Ops
  cme_error_t (*recv_callback)(cmsu_evl_t evl, ip_addr_t sender, buffer_t *buf,
                               void *ctx);
  cme_error_t (*send_callback)(cmsu_evl_t evl, ip_addr_t recver, buffer_t *buf,
                               void *ctx);
  void(*ctx_destroy);
} socket_udp_create_arg;
cme_error_t socket_udp_create(socket_udp_create_arg args, socket_t *out);

cme_error_t socket_udp_recv(cmsu_evl_t evl, socket_t socket);

cme_error_t socket_udp_send(cmsu_evl_t evl, ip_addr_t recver, buffer_t buf,
                            socket_t socket);

#endif // C_MINILIB_SIP_UA_SOCKET_H
