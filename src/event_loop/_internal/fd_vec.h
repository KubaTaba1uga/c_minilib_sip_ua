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
#include "utils/stc_wrapper.h"

#define i_tag _PollFdsVec
#define i_key __PollFd
#define i_cmp __PollFd_cmp
#include <stc/vec.h>

typedef struct vec__PollFdsVec __PollFdsVec;

// Underlaying vector can be reallocated, so we are not returning any indicator
// for element position. If we would take index in case some other element is
// dropped or inserted before this index we would have bug. Similiar to ptr,
// if we take ptr to element and array got shifted value under ptr would change.
// It's safest to just search threw vector in case we need sth from it.
static inline cme_error_t __PollFdsVec_push(vec__PollFdsVec *fds,
                                            __PollFd new) {
  __PollFd *result = vec__PollFdsVec_push(fds, new);
  if (!result) {
    return cme_return(cme_error(ENOMEM, "Unable to add new poll fd"));
  }

  return 0;
}

static inline cme_error_t __PollFdsVec_poll(vec__PollFdsVec *fds) {

  int result = poll(fds->data, fds->size, -1);
  if (result < 0) {
    return cme_return(cme_error(ENOMEM, "Unable to poll fd"));
  }
  return 0;
}

static inline __PollFd *__PollFdsVec_find(int32_t fd, vec__PollFdsVec *fds) {
  vec_foreach(vec_fd, vec__PollFdsVec, *fds) {
    if (vec_fd.ref->fd == fd) {
      return (__PollFd *)vec_fd.ref;
    }
  }

  return NULL;
}

static inline void __PollFdsVec_remove(int32_t fd, vec__PollFdsVec *fds) {
  vec_foreach(vec_fd, vec__PollFdsVec, *fds) {
    if (vec_fd.ref->fd == fd) {
      vec__PollFdsVec_erase_at(fds, vec_fd);
      break;
    }
  }
}

/* static inline void __PollFdsVec_remove(int32_t fd, vec__PollFdsVec *fds) { */
/*   c_foreach(vec_fd, vec__PollFdsVec, *fds) { */
/*     if (vec_fd.ref->fd == fd) { */
/*       vec__PollFdsVec_erase_at(fds, vec_fd); */
/*       break; */
/*     } */
/*   } */
/* } */

static inline cme_error_t __PollFdsVec_set_pollout(int32_t fd,
                                                   vec__PollFdsVec *fds) {
  __PollFd *out = __PollFdsVec_find(fd, fds);
  if (!out) {
    return cme_return(
        cme_errorf(ENOMEM, "Cannot find fd in event loop: %d", fd));
  }

  out->events |= POLLOUT;

  return 0;
};

static inline cme_error_t __PollFdsVec_set_pollin(int32_t fd,
                                                  vec__PollFdsVec *fds) {
  __PollFd *out = __PollFdsVec_find(fd, fds);
  if (!out) {
    return cme_return(
        cme_errorf(ENOMEM, "Cannot find fd in event loop: %d", fd));
  }

  out->events |= POLLIN;

  return 0;
};

static inline cme_error_t __PollFdsVec_unset_pollout(int32_t fd,
                                                     vec__PollFdsVec *fds) {
  __PollFd *out = __PollFdsVec_find(fd, fds);

  if (!out) {
    return cme_return(
        cme_errorf(ENOMEM, "Cannot find fd in event loop: %d", fd));
  }

  out->events &= ~POLLOUT;

  return 0;
};

#endif
