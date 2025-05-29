#ifndef C_MINILIB_SIP_UA_POLL_FD_VEC_H
#define C_MINILIB_SIP_UA_POLL_FD_VEC_H

#include "c_minilib_error.h"
#include "event_loop/_internal/poll_fd.h"

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

#endif
