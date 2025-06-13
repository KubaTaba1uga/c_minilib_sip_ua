/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_TIMER_H
#define C_MINILIB_SIP_UA_INT_TIMER_H
#include <stdlib.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sys/timerfd.h"
#include "timer/timer.h"

struct cmsu_Timer {
  // Event loop data
  event_loop_t evl;
  int32_t fd;

  // Timer data
  bool do_cleanup;

  // User data & ops
  timer_timeouth_t timeouth;
  void *timeouth_arg;
};

cme_error_t cmsu_Timer_timeouth(void *data) {
  mytimer_t timer = data;
  cme_error_t err;

  timer->do_cleanup = true;

  err = timer->timeouth(timer, timer->timeouth_arg);

  if (timer->do_cleanup) {
    event_loop_remove_fd(timer->evl, timer->fd);
    close(timer->fd);
  }

  return err;
};

cme_error_t cmsu_Timer_create(event_loop_t evl, time_t seconds, long nseconds,
                              timer_timeouth_t timeouth, void *timeouth_arg,
                              mytimer_t *out) {
  struct cmsu_Timer *timer = calloc(1, sizeof(struct cmsu_Timer));
  cme_error_t err;
  if (!timer) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `timer`");
    goto error_out;
  }

  errno = 0;
  int timerfd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
  if (timerfd == -1) {
    err = cme_error(errno, "OS cannot create timer");
    goto error_timerfd_cleanup;
  }

  struct itimerspec clock_value = {0};
  clock_value.it_value.tv_sec = seconds;
  clock_value.it_value.tv_nsec = nseconds;

  errno = 0;
  if (timerfd_settime(timerfd, 0, &clock_value, NULL) != 0) {
    err = cme_error(errno, "OS cannot configure timer");
    goto error_timerfd_cleanup;
  };

  err = event_loop_insert_timerfd(evl, (fd_t){.fd = timerfd},
                                  cmsu_Timer_timeouth, timer);
  if (err) {
    err = cme_errorf(errno, "Cannot insert timer into event loop");
    goto error_socket_cleanup;
  }

  err = event_loop_fd_set_pollin(evl, timerfd);
  if (err) {
    err = cme_errorf(errno, "Cannot set POLLIN on timer in event loop");
    goto error_socket_cleanup;
  }

  timer->evl = evl;
  timer->fd = timerfd;
  timer->timeouth = timeouth;
  timer->timeouth_arg = timeouth_arg;

  *out = timer;

  return 0;

error_socket_cleanup:
  close(timerfd);
error_timerfd_cleanup:
  free(timer);
error_out:
  *out = NULL;
  return err;
};

#endif // C_MINILIB_SIP_UA_UDP_H
