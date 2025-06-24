/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_INT_FD_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_INT_FD_H

#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>

typedef struct pollfd __PollFd;

static inline __PollFd __PollFd_clone(__PollFd src) { return src; }

static inline void __PollFd_drop(__PollFd *self) {};

static inline int __PollFd_cmp(const __PollFd *a, const __PollFd *b) {
  if (a->fd == b->fd) {
    return 0;
  }
  if (a->fd > b->fd) {
    return 1;
  }
  return -1;
}

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_INT_FD_H
