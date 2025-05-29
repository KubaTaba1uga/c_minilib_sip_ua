/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SOCKET_H
#define C_MINILIB_SIP_UA_INT_SOCKET_H

#include "c_minilib_error.h"
#include "utils/error.h"
#include <unistd.h>

enum cmsu_SupportedSockets {
  cmsu_SupportedSockets_UDP,
};

struct cmsu_Socket {
  // Data
  enum cmsu_SupportedSockets proto;
  void *ctx;
  cme_error_t (*sendh)(void *ctx);
  cme_error_t (*recvh)(void *ctx);
  void (*destroyh)(void *ctx);
  int (*get_fd_func)(void *ctx);
};

typedef struct cmsu_Socket cmsu_Socket;

static inline int cmsu_Socket_cmp(const struct cmsu_Socket *a,
                                  const struct cmsu_Socket *b) {
  int a_fd = a->get_fd_func(a->ctx);
  int b_fd = b->get_fd_func(b->ctx);

  if (a_fd == b_fd) {
    return 0;
  }

  if (a_fd > b_fd) {
    return 1;
  }

  return -1;
}

static inline void cmsu_Socket_drop(struct cmsu_Socket *self) {
  close(self->get_fd_func(self->ctx));
  self->destroyh(self->ctx);
  free(self);
};

static inline void cmsu_Socket_destroy(struct cmsu_Socket *self) {
  cmsu_Socket_drop(self);
};

#endif // C_MINILIB_SIP_UA_SOCKET_H
