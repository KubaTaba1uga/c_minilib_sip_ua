/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_TIMER_H
#define C_MINILIB_SIP_UA_TIMER_H
/*
  This is interface to timer module, if you need anything from timer
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "utils/ip.h"
#include <stdint.h>
#include <time.h>

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
typedef struct cmsu_Timer *mytimer_t;
typedef cme_error_t (*timer_timeouth_t)(mytimer_t timer, void *data);

cme_error_t mytimer_create(event_loop_t evl, time_t seconds, long nseconds,
                           timer_timeouth_t timeouth, void *timeouth_arg,
                           mytimer_t *out);
void mytimer_destroy(mytimer_t *out);

#endif // C_MINILIB_SIP_UA_UDP_H
