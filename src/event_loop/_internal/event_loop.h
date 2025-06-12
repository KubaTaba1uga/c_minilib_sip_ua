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

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/common.h"
#include "event_loop/_internal/fd_helper.h"
#include "event_loop/_internal/fd_helper_hmap.h"
#include "event_loop/_internal/fd_vec.h"
#include "stc/common.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/

struct cmsu_EventLoop {
  struct vec_cmsu_Fds fds;
  struct hmap_cmsu_FdHelpers fds_helpers;
};

static inline cme_error_t
cmsu_EventLoop_process_events(struct cmsu_EventLoop *evl);

static inline cme_error_t cmsu_EventLoop_create(struct cmsu_EventLoop **out) {
  struct cmsu_EventLoop *evl = calloc(1, sizeof(struct cmsu_EventLoop));
  cme_error_t err;
  if (!evl) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `evl`");
    goto error_out;
  }

  evl->fds = vec_cmsu_Fds_init();
  evl->fds_helpers = hmap_cmsu_FdHelpers_init();

  *out = evl;

  return 0;

error_out:
  *out = NULL;
  return cme_return(err);
};

static inline cme_error_t cmsu_EventLoop_start(struct cmsu_EventLoop *evl) {
  cme_error_t err;

  while (true) {
    err = my_vec_cmsu_Fds_poll(&evl->fds);
    if (err) {
      goto error_out;
    }

    err = cmsu_EventLoop_process_events(evl);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsu_EventLoop_process_events(struct cmsu_EventLoop *evl) {
  cme_error_t err;

  c_foreach(fd, vec_cmsu_Fds, evl->fds) {
    struct cmsu_FdHelper *fd_helper =
        my_hmap_cmsu_FdHelpers_find(fd.ref->fd, &evl->fds_helpers);

    assert(fd_helper != NULL);

    if (fd.ref->revents & POLLIN) {
      err = cmsu_FdHelper_recvh(fd_helper);
      if (err) {
        goto error_out;
      }
    }
    if (fd.ref->revents & POLLOUT) {
      err = cmsu_FdHelper_sendh(fd_helper);
      if (err) {
        goto error_out;
      }
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

void cmsu_EventLoop_destroy(struct cmsu_EventLoop **evl) {
  if (!evl || !*evl) {
    return;
  }

  vec_cmsu_Fds_drop(&(*evl)->fds);
  hmap_cmsu_FdHelpers_drop(&(*evl)->fds_helpers);

  free(*evl);
  *evl = NULL;
};

cme_error_t cmsu_EventLoop_insert_fd(struct cmsu_EventLoop *evl, fd_t fd,
                                     event_loop_sendh_t sendh,
                                     event_loop_recvh_t recvh, void *data) {
  cme_error_t err;
  err = my_vec_cmsu_Fds_push(&evl->fds, fd);
  if (err) {
    goto error_out;
  }

  fd_helper_t helper;
  err = cmsu_FdHelper_create(sendh, recvh, data, &helper);
  if (err) {
    goto error_fds_cleanup;
  }

  err = my_hmap_cmsu_FdHelpers_insert(fd.fd, helper, &evl->fds_helpers);
  if (err) {
    goto error_helper_cleanup;
  }

  return 0;

error_helper_cleanup:
  cmsu_FdHelper_destroy(&helper);
error_fds_cleanup:
  my_vec_cmsu_Fds_remove(fd.fd, &evl->fds);
error_out:
  return cme_return(err);
}

void cmsu_EventLoop_remove_fd(event_loop_t evl, int32_t fd) {
  my_vec_cmsu_Fds_remove(fd, &evl->fds);
  my_hmap_cmsu_FdHelpers_remove(fd, &evl->fds_helpers);
}

#endif // C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
