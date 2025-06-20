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
#include "timer_fd/timer_fd.h"

struct __TimerFd {
  // Event loop data
  event_loop_t evl;
  int32_t fd;

  // Timer data
  bool do_cleanup;

  // User data & ops
  timer_fd_timeouth_t timeouth;
  void *timeouth_arg;
};

static inline void __TimerFd_destroy(struct __TimerFd *timerp);
static inline struct __TimerFd __TimerFd_clone(struct __TimerFd timerp);

#define i_type __TimerFdPtr
#define i_key struct __TimerFd
#define i_keydrop __TimerFd_destroy
#define i_keyclone __TimerFd_clone
#include "stc/arc.h"

cme_error_t __Timer_timeouth(void *data) {
  timer_fd_t timer = data;
  cme_error_t err;

  timer->get->do_cleanup = true;

  err = timer->get->timeouth(timer, timer->get->timeouth_arg);
  if (err || timer->get->do_cleanup) {
    event_loop_remove_fd(timer->get->evl, timer->get->fd);
    close(timer->get->fd);
  }

  return err;
};

cme_error_t __Timer_create(event_loop_t evl, time_t seconds, long nseconds,
                           timer_fd_timeouth_t timeouth, void *timeouth_arg,
                           timer_fd_t *out) {
  timer_fd_t timerp = malloc(sizeof(struct __TimerFdPtr));
  cme_error_t err;
  if (!timerp) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `struct __TimerFdPtr`");
    goto error_out;
  }

  errno = 0;
  int timerfd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
  if (timerfd == -1) {
    err = cme_error(errno, "OS cannot create timer");
    goto error_timerp_cleanup;
  }

  struct itimerspec clock_value = {0};
  clock_value.it_value.tv_sec = seconds;
  clock_value.it_value.tv_nsec = nseconds;

  errno = 0;
  if (timerfd_settime(timerfd, 0, &clock_value, NULL) != 0) {
    err = cme_error(errno, "OS cannot configure timer");
    goto error_timerp_cleanup;
  };

  err = event_loop_insert_timerfd(evl, timerfd, __Timer_timeouth, timerp);
  if (err) {
    err = cme_errorf(errno, "Cannot insert timer into event loop");
    goto error_timerfd_cleanup;
  }

  err = event_loop_set_pollin(evl, timerfd);
  if (err) {
    err = cme_errorf(errno, "Cannot set POLLIN on timer in event loop");
    goto error_timerfd_cleanup;
  }

  *timerp = __TimerFdPtr_from((struct __TimerFd){.evl = event_loop_ref(evl),
                                                 .timeouth = timeouth,
                                                 .timeouth_arg = timeouth_arg,
                                                 .fd = timerfd,
                                                 .do_cleanup = false});
  *out = timerp;

  return 0;

error_timerfd_cleanup:
  close(timerfd);
error_timerp_cleanup:
  free(timerp);
error_out:
  *out = NULL;
  return err;
};

static inline void __TimerFd_destroy(struct __TimerFd *timerp) {
  event_loop_deref(timerp->evl);
};

static inline struct __TimerFd __TimerFd_clone(struct __TimerFd timerp) {
  return timerp;
};

static inline timer_fd_t __TimerFd_ref(timer_fd_t timerp) {

  __TimerFdPtr_clone(*timerp);

  return timerp;
}

static inline void __TimerFd_deref(timer_fd_t timerp) {
  int32_t usage_count = __TimerFdPtr_use_count(timerp);

  __TimerFdPtr_drop(timerp);

  // If usage count is 1 before drop it means it will be 0
  //  after drop but ptr holding usage count get freed on drop.
  if (usage_count <= 1) {
    free(timerp);
  }
}

static inline void __TimerFd_rearm(timer_fd_t timerp) {
  timerp->get->do_cleanup = false;
}

#endif // C_MINILIB_SIP_UA_INT_TIMER_FD_H
