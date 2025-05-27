#ifndef C_MINILIB_SIP_UA_POLL_H
#define C_MINILIB_SIP_UA_POLL_H

#include <errno.h>
#include <stc/cstr.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "c_minilib_mock.h"

#include "event_loop/poll/pollfds.h"

static inline cme_error_t cmsu_poll(cmsu_PollFds *pollfds) {
  int result = poll(pollfds->data, pollfds->size, -1);
  if (result < 0) {
    return cme_return(cme_error(ENOMEM, "Unable to poll fd"));
  }
  return 0;
}

#endif
