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
#include "event_loop/_internal/fd.h"
#include "event_loop/_internal/fd_helper.h"
#include "event_loop/_internal/fd_helper_hmap.h"
#include "event_loop/_internal/fd_vec.h"
#include "event_loop/event_loop.h"
#include "stc/common.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
struct __EventLoop {
  __PollFdsVec fds;
  __FdHelpersMap fds_helpers;
};

typedef struct __EventLoop __EventLoop;

cme_error_t __EventLoop_create(event_loop_t *out) {
  __EventLoop *evl = malloc(sizeof(__EventLoop));
  cme_error_t err;
  if (!evl) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `evl`");
    goto error_out;
  }

  evl->fds = vec__PollFdsVec_init();
  evl->fds_helpers = hmap__FdHelpersMap_init();

  *out = __EventLoopPtr_from(evl);

  return 0;

error_out:
  *out = (__EventLoopPtr){0};
  return cme_return(err);
};

static inline cme_error_t __EventLoop_process_events(event_loop_t evl);

static inline cme_error_t __EventLoop_start(event_loop_t evl) {
  cme_error_t err;

  while (true) {
    err = __PollFdsVec_poll(&(*evl.get)->fds);
    if (err) {
      goto error_out;
    }

    err = __EventLoop_process_events(evl);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t __EventLoop_process_events(event_loop_t evl) {
  cme_error_t err;

  c_foreach(fd, vec__PollFdsVec, (*evl.get)->fds) {
    __FdHelper *fd_helper =
        my_hmap___FdHelpersMap_find(fd.ref->fd, &(*evl.get)->fds_helpers);

    assert(fd_helper != NULL);

    if (fd.ref->revents & POLLIN) {
      err = __FdHelper_pollinh(fd_helper);
      if (err) {
        goto error_out;
      }
    }

    if (fd.ref->revents & POLLOUT) {
      err = __FdHelper_pollouth(fd_helper);
      if (err) {
        goto error_out;
      }
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t __EventLoop_insert_socketfd(event_loop_t evl, uint32_t fd,
                                        event_loop_sendh_t sendh,
                                        event_loop_recvh_t recvh, void *data) {
  cme_error_t err;
  err = __PollFdsVec_push(&(*evl.get)->fds,
                          (__PollFd){.fd = fd, .events = 0, .revents = 0});
  if (err) {
    goto error_out;
  }

  err = __FdHelpersMap_insert(fd,
                              (__FdHelper){.fd_type = __FdType_SOCKET,
                                           .timeouth = NULL,
                                           .sendh = sendh,
                                           .recvh = recvh,
                                           .data = data},
                              &(*evl.get)->fds_helpers);
  if (err) {
    goto error_fds_cleanup;
  }

  return 0;

error_fds_cleanup:
  __PollFdsVec_remove(fd, &(*evl.get)->fds);
error_out:
  return cme_return(err);
}

cme_error_t __EventLoop_insert_timerfd(event_loop_t evl, uint32_t fd,
                                       event_loop_timeouth_t timeouth,
                                       void *data) {
  cme_error_t err;
  err = __PollFdsVec_push(&(*evl.get)->fds,
                          (__PollFd){.fd = fd, .events = 0, .revents = 0});
  if (err) {
    goto error_out;
  }

  err = __FdHelpersMap_insert(fd,
                              (__FdHelper){.fd_type = __FdType_TIMER,
                                           .timeouth = timeouth,
                                           .sendh = NULL,
                                           .recvh = NULL,
                                           .data = data},
                              &(*evl.get)->fds_helpers);
  if (err) {
    goto error_fds_cleanup;
  }

  return 0;

error_fds_cleanup:
  __PollFdsVec_remove(fd, &(*evl.get)->fds);
error_out:
  return cme_return(err);
}

event_loop_t __EventLoop_ref(event_loop_t evl) {
  return __EventLoopPtr_clone(evl);
};

void __EventLoop_deref(event_loop_t *evl) { __EventLoopPtr_drop(evl); };

void __EventLoop_destructor(__EventLoopRaw **evlp) {
  vec__PollFdsVec_drop(&(*evlp)->fds);
  hmap__FdHelpersMap_drop(&(*evlp)->fds_helpers);
};

#endif
