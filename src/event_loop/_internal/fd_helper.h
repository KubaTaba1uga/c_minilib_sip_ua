/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_FD_HELPER_H
#define C_MINILIB_SIP_UA_INT_FD_HELPER_H

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

static inline void cmsu_FdHelper_drop(struct cmsu_FdHelper *self) {
  /* self->sendh = NULL; */
  /* self->recvh = NULL; */
  // do NOT free self->data
  /* self->data = NULL; */
}

static inline struct cmsu_FdHelper
cmsu_FdHelper_clone(struct cmsu_FdHelper src) {
  return src;
}

#endif
