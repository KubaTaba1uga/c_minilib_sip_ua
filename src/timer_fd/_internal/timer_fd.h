/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_TIMER_FD_H
#define C_MINILIB_SIP_UA_INT_TIMER_FD_H
#include <stdlib.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sys/timerfd.h"

struct TimerFdPtr;
typedef cme_error_t (*timer_fd_timeouth_t)(struct TimerFdPtr *timer,
                                           void *data);

struct __TimerFd {
  // Event loop data
  struct EventLoopPtr evl;
  int32_t fd;

  // Timer data
  bool do_cleanup;

  // User data & ops
  timer_fd_timeouth_t timeouth;
  void *timeouth_arg;
};

void __TimerFd_destroy(struct __TimerFd *timerp);
struct __TimerFd __TimerFd_clone(struct __TimerFd timerp);

#endif // C_MINILIB_SIP_UA_INT_TIMER_FD_H
