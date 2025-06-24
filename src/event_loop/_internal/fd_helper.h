/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_INT_FD_HELPER_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_INT_FD_HELPER_H

#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/generic_ptr.h"

/******************************************************************************
 *                             Fd Helper                                      *
 ******************************************************************************/
typedef cme_error_t (*event_loop_recvh_t)(struct GenericPtr data);
typedef cme_error_t (*event_loop_timeouth_t)(struct GenericPtr data);

enum __FdType {
  __FdType_SOCKET,
  __FdType_TIMER,
};

struct __FdHelper {
  event_loop_timeouth_t timeouth; // This is used by timer
  event_loop_recvh_t recvh;       // This is used by socket
  enum __FdType fd_type;          // This is used by socket and timer
  struct GenericPtr data;         // This is used by socket and timer
};

typedef struct __FdHelper __FdHelper;

static inline cme_error_t __FdHelper_init(enum __FdType fd_type,
                                          event_loop_timeouth_t timeouth,
                                          event_loop_recvh_t recvh,
                                          struct GenericPtr data,
                                          __FdHelper *out) {
  out->timeouth = timeouth;
  out->fd_type = fd_type;
  out->recvh = recvh;
  out->data = data;

  return 0;
}

static inline void __FdHelper_destroy(__FdHelper **out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  free(*out);
  *out = NULL;
}

static inline cme_error_t __FdHelper_pollinh(__FdHelper *helper) {
  switch (helper->fd_type) {
  case __FdType_SOCKET:
    return helper->recvh(helper->data);
  case __FdType_TIMER:
    return helper->timeouth(helper->data);
  default:
    return 0;
  }
}

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_INT_FD_HELPER_H
