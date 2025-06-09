/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_POLL_FD_VEC_H
#define C_MINILIB_SIP_UA_POLL_FD_VEC_H

#include "c_minilib_error.h"
#include "event_loop/_internal/fd.h"
#include <stdint.h>

#define i_tag fds_t
#define i_keyclass fd_t
#define i_cmp fd_cmp
#include <stc/vec.h>

typedef struct vec_fds_t vec_fds_t;

// Underlaying vector can be reallocated, so we are not returning any indicator
// for element position. If we would take index in case some other element is
// dropped or inserted before this index we would have bug. Similiar to ptr,
// if we take ptr to element and array got shifted value under ptr would change.
// It's safest to just search threw vector in case we need sth from it.
static inline cme_error_t my_vec_fds_t_push(vec_fds_t *fds, fd_t new) {
  fd_t *result = vec_fds_t_push(fds, new);
  if (!result) {
    return cme_return(cme_error(ENOMEM, "Unable to add new poll fd"));
  }

  return 0;
}

static inline cme_error_t my_vec_fds_t_poll(vec_fds_t *fds) {

  int result = poll(fds->data, fds->size, -1);
  if (result < 0) {
    return cme_return(cme_error(ENOMEM, "Unable to poll fd"));
  }
  return 0;
}

static inline struct fd_t *my_vec_fds_t_find(int32_t fd, vec_fds_t *fds) {
  c_foreach(vec_fd, vec_fds_t, *fds) {
    if (vec_fd.ref->fd == fd) {
      return (struct fd_t *)vec_fd.ref;
    }
  }

  return NULL;
}

static inline void my_vec_fds_t_remove(int32_t fd, vec_fds_t *fds) {
  c_foreach(vec_fd, vec_fds_t, *fds) {
    if (vec_fd.ref->fd == fd) {
      vec_fds_t_erase_at(fds, vec_fd);
      break;
    }
  }
}

#endif
