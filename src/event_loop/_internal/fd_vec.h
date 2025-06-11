/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_POLL_FD_VEC_H
#define C_MINILIB_SIP_UA_POLL_FD_VEC_H
#include <stdint.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/fd.h"
#include "event_loop/event_loop.h"

#define i_tag cmsu_Fds
#define i_key fd_t
#define i_cmp fd_cmp
#include <stc/vec.h>

typedef struct vec_cmsu_Fds vec_cmsu_Fds;

// Underlaying vector can be reallocated, so we are not returning any indicator
// for element position. If we would take index in case some other element is
// dropped or inserted before this index we would have bug. Similiar to ptr,
// if we take ptr to element and array got shifted value under ptr would change.
// It's safest to just search threw vector in case we need sth from it.
static inline cme_error_t my_vec_cmsu_Fds_push(vec_cmsu_Fds *fds, fd_t new) {
  fd_t *result = vec_cmsu_Fds_push(fds, new);
  if (!result) {
    return cme_return(cme_error(ENOMEM, "Unable to add new poll fd"));
  }

  return 0;
}

static inline cme_error_t my_vec_cmsu_Fds_poll(vec_cmsu_Fds *fds) {

  int result = poll(fds->data, fds->size, -1);
  if (result < 0) {
    return cme_return(cme_error(ENOMEM, "Unable to poll fd"));
  }
  return 0;
}

static inline fd_t *my_vec_cmsu_Fds_find(int32_t fd, vec_cmsu_Fds *fds) {
  c_foreach(vec_fd, vec_cmsu_Fds, *fds) {
    if (vec_fd.ref->fd == fd) {
      return (fd_t *)vec_fd.ref;
    }
  }

  return NULL;
}

static inline void my_vec_cmsu_Fds_remove(int32_t fd, vec_cmsu_Fds *fds) {
  c_foreach(vec_fd, vec_cmsu_Fds, *fds) {
    if (vec_fd.ref->fd == fd) {
      vec_cmsu_Fds_erase_at(fds, vec_fd);
      break;
    }
  }
}

static inline cme_error_t my_vec_cmsu_Fds_set_pollout(int32_t fd,
                                                      vec_cmsu_Fds *fds) {
  fd_t *out = my_vec_cmsu_Fds_find(fd, fds);
  if (!out) {
    return cme_return(
        cme_errorf(ENOMEM, "Cannot find fd in event loop: %d", fd));
  }

  out->events |= POLLOUT;

  return 0;
};

static inline cme_error_t my_vec_cmsu_Fds_set_pollin(int32_t fd,
                                                     vec_cmsu_Fds *fds) {
  fd_t *out = my_vec_cmsu_Fds_find(fd, fds);
  if (!out) {
    return cme_return(
        cme_errorf(ENOMEM, "Cannot find fd in event loop: %d", fd));
  }

  out->events |= POLLIN;

  return 0;
};

static inline cme_error_t my_vec_cmsu_Fds_unset_pollin(int32_t fd,
                                                       vec_cmsu_Fds *fds) {
  fd_t *out = my_vec_cmsu_Fds_find(fd, fds);

  if (!out) {
    return cme_return(
        cme_errorf(ENOMEM, "Cannot find fd in event loop: %d", fd));
  }

  out->events &= ~POLLIN;

  return 0;
};

#endif
