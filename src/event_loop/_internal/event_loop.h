/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "stc/common.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
typedef struct {
} __EventLoop;

__EventLoop __EventLoop_make() { return (__EventLoop){}; }

__EventLoop __EventLoop_clone(__EventLoop evl) { return evl; }

void __EventLoop_drop(__EventLoop *evl) { c_drop(__EventLoop, evl); }

#define i_type EventLoop
#define i_keyclass __EventLoop
#include "stc/arc.h"

#endif
