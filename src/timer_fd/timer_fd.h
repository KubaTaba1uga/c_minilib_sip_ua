/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_TIMER_FD_H
#define C_MINILIB_SIP_UA_TIMER_FD_H
/*
  This is interface to timer module, if you need anything from timer
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */
#include <stdint.h>
#include <time.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "utils/ip.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
typedef struct __TimerFdPtr *timer_fd_t;
typedef cme_error_t (*timer_fd_timeouth_t)(timer_fd_t timer, void *data);

cme_error_t timer_fd_create(event_loop_t evl, time_t seconds, long nseconds,
                            timer_fd_timeouth_t timeouth, void *timeouth_arg,
                            timer_fd_t *out);

timer_fd_t timer_fd_ref(timer_fd_t timerp);

void timer_fd_deref(timer_fd_t timerp);

#endif // C_MINILIB_SIP_UA_TIMER_FD_H
