/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
/*
  This is interface to int_event_loop module, if you need anything from
  int_event_loop module, put interface declaration here and interface
  implementation in .c but always write real implementation as static inline in
  _internal.
 */

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "stc/common.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/

struct cmsu_EventLoop {};

/* static inline cme_error_t */
/* cmsu_EventLoop_process_events(struct cmsu_EventLoop *evl); */

static inline cme_error_t cmsu_EventLoop_create(struct cmsu_EventLoop **out) {
  struct cmsu_EventLoop *evl = calloc(1, sizeof(struct cmsu_EventLoop));
  cme_error_t err;
  if (!evl) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `evl`");
    goto error_out;
  }

  *out = evl;

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsu_EventLoop_start(struct cmsu_EventLoop *evl) {
  cme_error_t err;
  /* int poll_res; */

  while (true) {
    /* poll_res = poll(evl->fds.data, evl->fds.size, -1); */
    /* if (poll_res < 0) { */
    /*   err = cme_error(EIO, "Polling error"); */
    /*   goto error_out; */
    /* } */

    /* err = cmsu_EventLoop_process_events(evl); */
    /* if (err) { */
    /* goto error_out; */
    /* } */
  }

  return 0;

error_out:
  return cme_return(err);
};

void cmsu_EventLoop_destroy(struct cmsu_EventLoop **evl) {
  if (!evl || !*evl) {
    return;
  }

  free(*evl);
  *evl = NULL;
};

#endif // C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
