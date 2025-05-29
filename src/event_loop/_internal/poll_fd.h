/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_POLL_FD_H
#define C_MINILIB_SIP_UA_POLL_FD_H

#include <sys/poll.h>

typedef struct pollfd cmsu_PollFd;

static inline int cmsu_PollFd_cmp(const cmsu_PollFd *a, const cmsu_PollFd *b) {
  if (a->fd == b->fd) {
    return 0;
  }
  if (a->fd > b->fd) {
    return 1;
  }
  return -1;
}

static inline cmsu_PollFd cmsu_PollFd_clone(cmsu_PollFd src) {
  return (cmsu_PollFd){
      .fd = src.fd, .events = src.events, .revents = src.revents};
}

void cmsu_PollFd_drop(cmsu_PollFd *self);

#endif
