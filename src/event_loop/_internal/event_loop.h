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
#include "stc/common.h"
#include "stc/cstr.h"

#include "event_loop/_internal/fd_helper_hmap.h"
#include "event_loop/_internal/fd_vec.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
struct __EventLoop {
  __PollFdsVec fds;
  __FdHelpersMap fds_helpers;
};

void __EventLoop_destroy(struct __EventLoop *evl);
struct __EventLoop __EventLoop_clone(struct __EventLoop evl);

#define i_type __EventLoopPtr
#define i_key struct __EventLoop
#define i_keydrop __EventLoop_destroy
#define i_keyclone __EventLoop_clone
#include "stc/arc.h"

#endif
