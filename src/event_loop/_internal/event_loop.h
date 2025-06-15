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
struct __EventLoop {
  int32_t dummy_int;
};

cme_error_t __EventLoop_create(event_loop_t *out) {
  struct __EventLoop *evl = calloc(1, sizeof(struct __EventLoop));
  cme_error_t err;
  if (!evl) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `evl`");
    goto error_out;
  }

  /* evl->fds = vec_cmsu_Fds_init(); */
  /* evl->fds_helpers = hmap_cmsu_FdHelpers_init(); */

  *out = __EventLoopPtr_from(evl);

  return 0;

error_out:
  *out = (struct __EventLoopPtr){0};
  return cme_return(err);
};

#endif
