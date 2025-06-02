/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_POLL_FD_VEC_H
#define C_MINILIB_SIP_UA_POLL_FD_VEC_H

#include "c_minilib_error.h"
#include "event_loop/_internal/poll_fd.h"
#include "socket/socket.h"
#include <stdint.h>

#define i_tag cmsu_PollFds
#define i_keyclass cmsu_PollFd
#define i_cmp cmsu_PollFd_cmp
#include <stc/vec.h>

typedef struct vec_cmsu_PollFds cmsu_PollFds;

// Underlaying vector can be reallocated, so we are not returning any indicator
// for element position. If we would take index in case some other element is
// dropped or inserted before this index we would have bug. Similiar to ptr,
// if we take ptr to element and array got shifted value under ptr would change.
// It's safest to just search threw vector in case we need sth from it.
static inline cme_error_t cmsu_PollFds_push(cmsu_PollFds *pollfds,
                                            cmsu_PollFd value) {
  cmsu_PollFd *result = vec_cmsu_PollFds_push(pollfds, value);
  if (!result) {
    return cme_return(cme_error(ENOMEM, "Unable to add new poll fd"));
  }

  return 0;
}

static inline cme_error_t cmsu_PollFds_poll(cmsu_PollFds *pollfds) {

  int result = poll(pollfds->data, pollfds->size, -1);
  if (result < 0) {
    return cme_return(cme_error(ENOMEM, "Unable to poll fd"));
  }
  return 0;
}

static inline struct cmsu_PollFd *cmsu_PollFds_find(int32_t fd,
                                                    cmsu_PollFds *pollfds) {
  c_foreach(vec_fd, vec_cmsu_PollFds, *pollfds) {
    if (vec_fd.ref->fd == fd) {
      return (struct cmsu_PollFd *)vec_fd.ref;
    }
  }

  return 0;
}

static inline void cmsu_PollFds_remove(int32_t fd, cmsu_PollFds *pollfds) {
  c_foreach(vec_fd, vec_cmsu_PollFds, *pollfds) {
    if (vec_fd.ref->fd == fd) {
      vec_cmsu_PollFds_erase_at(pollfds, vec_fd);
      break;
    }
  }
}

#endif
