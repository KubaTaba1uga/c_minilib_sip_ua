/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_H
/*
  This is interface to event_loop module, if you need anything from event_loop
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */

#include <poll.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "c_minilib_error.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/

typedef struct cmsu_EventLoop *EventLoop_t;
typedef cme_error_t (*EventLoop_sendh_t)(void *data);
typedef cme_error_t (*EventLoop_recvh_t)(void *data);
typedef cme_error_t (*EventLoop_timeouth_t)(void *data);

cme_error_t EventLoop_create(EventLoop_t out);
cme_error_t EventLoop_ref(EventLoop_t evl);
cme_error_t EventLoop_deref(EventLoop_t evl);

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
