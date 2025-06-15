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
#include <sys/poll.h>

#include "c_minilib_error.h"

// Helper to prettify this header
#include "event_loop/event_loop_ptr.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
typedef struct __EventLoopPtr event_loop_t;
typedef cme_error_t (*event_loop_sendh_t)(void *data);
typedef cme_error_t (*event_loop_recvh_t)(void *data);
typedef cme_error_t (*event_loop_timeouth_t)(void *data);

cme_error_t event_loop_create(event_loop_t *out);
event_loop_t event_loop_ref(event_loop_t evl);
void event_loop_deref(event_loop_t evl);

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
