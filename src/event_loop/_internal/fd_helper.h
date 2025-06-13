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
#include <stdlib.h>

/******************************************************************************
 *                             Fd Helper                                      *
 ******************************************************************************/
enum cmsu_FdType {
  cmsu_FdType_SOCKET,
  cmsu_FdType_TIMER,
};

struct cmsu_FdHelper {
  event_loop_timeouth_t timeouth; // This is used by timer
  event_loop_sendh_t sendh;       // This is used by socket
  event_loop_recvh_t recvh;       // This is used by socket
  enum cmsu_FdType fd_type;       // This is used by socket and timer
  void *data;                     // This is used by socket and timer
};

static inline cme_error_t
cmsu_FdHelper_create(enum cmsu_FdType fd_type, event_loop_timeouth_t timeouth,
                     event_loop_sendh_t sendh, event_loop_recvh_t recvh,
                     void *data, struct cmsu_FdHelper **out) {
  struct cmsu_FdHelper *helper = calloc(1, sizeof(struct cmsu_FdHelper));
  cme_error_t err;
  if (!helper) {
    err = cme_error(ENOMEM, "Cannot create fd helper");
    goto error_out;
  }

  helper->timeouth = timeouth;
  helper->fd_type = fd_type;
  helper->sendh = sendh;
  helper->recvh = recvh;
  helper->data = data;

  *out = helper;

  return 0;

error_out:
  return cme_return(err);
}

static inline void cmsu_FdHelper_destroy(struct cmsu_FdHelper **out) {
  if (!out || !*out) {
    return;
  }

  free(*out);
  *out = NULL;
}

static inline cme_error_t cmsu_FdHelper_pollinh(struct cmsu_FdHelper *helper) {
  switch (helper->fd_type) {
  case cmsu_FdType_SOCKET:
    return helper->recvh(helper->data);
  case cmsu_FdType_TIMER:
    return helper->timeouth(helper->data);
  default:
    return 0;
  }
}

static inline cme_error_t cmsu_FdHelper_pollouth(struct cmsu_FdHelper *helper) {
  switch (helper->fd_type) {
  case cmsu_FdType_SOCKET:
    return helper->sendh(helper->data);
  default:
    return 0;
  }
}

#endif
