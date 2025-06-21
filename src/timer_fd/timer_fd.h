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

#include "timer_fd/_internal/timer_fd.h"

/******************************************************************************
 *                             Timer File Descriptor                          *
 ******************************************************************************/
#define i_type TimerFdPtr
#define i_key struct __TimerFd
#define i_keydrop __TimerFd_destroy
#define i_keyclone __TimerFd_clone
#include "stc/arc.h"

cme_error_t TimerFdPtr_create(struct EventLoopPtr evl, time_t seconds,
                              long nseconds, timer_fd_timeouth_t timeouth,
                              struct GenericPtr timeouth_arg,
                              struct TimerFdPtr *out);

void TimerFdPtr_rearm(struct TimerFdPtr timerp);

#endif // C_MINILIB_SIP_UA_TIMER_FD_H
