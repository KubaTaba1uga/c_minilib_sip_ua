/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_FD_H
#define C_MINILIB_SIP_UA_FD_H

#include <event_loop/event_loop.h>
#include <sys/poll.h>

static inline int fd_cmp(const fd_t *a, const fd_t *b) {
  if (a->fd == b->fd) {
    return 0;
  }
  if (a->fd > b->fd) {
    return 1;
  }
  return -1;
}

static inline fd_t fd_clone(fd_t src) { return src; }

void fd_drop(fd_t *self);

#endif
