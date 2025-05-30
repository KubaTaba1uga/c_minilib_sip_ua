/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SOCKET_H
#define C_MINILIB_SIP_UA_INT_SOCKET_H
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "socket/socket.h"
#include "utils/common.h"

/******************************************************************************
 *                                Socket                                      *
 ******************************************************************************/

enum cmsu_SupportedSocket {
  cmsu_SupportedSocket_NONE = 0,
  cmsu_SupportedSocket_UDP,
  cmsu_SupportedSocket_MAX,
};

struct cmsu_Socket {
  // Data
  enum cmsu_SupportedSocket type;
  ip_addr_t ipaddr;
  cmsu_evl_t evl;
  int sockfd;
  void *ctx;

  // Ops
  cme_error_t (*recv)(cmsu_evl_t evl, void *ctx);
  cme_error_t (*send)(cmsu_evl_t evl, void *ctx);
  void (*ctx_destroy)(cmsu_evl_t evl, void *ctx);
};

static inline cme_error_t cmsu_Socket_create(struct cmsu_Socket src,
                                             struct cmsu_Socket **out) {
  struct cmsu_Socket *socket = malloc(sizeof(struct cmsu_Socket));
  cme_error_t err;
  if (!socket) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `socket`");
    goto error_out;
  }

  *socket = src;
  *out = socket;

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_SOCKET_H
