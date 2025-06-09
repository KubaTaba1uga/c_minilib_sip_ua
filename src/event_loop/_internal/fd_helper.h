/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_STACK_H
#define C_MINILIB_SIP_UA_INT_SIP_STACK_H
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"

/******************************************************************************
 *                             Fd Helper                                      *
 ******************************************************************************/
struct cmsu_FdHelper {
  event_loop_sendh_t sendh;
  event_loop_recvh_t recvh;
  void *data;
};

#endif
