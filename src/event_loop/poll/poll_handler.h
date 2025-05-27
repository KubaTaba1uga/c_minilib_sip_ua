#ifndef C_MINILIB_SIP_UA_POLLFDS_H
#define C_MINILIB_SIP_UA_POLLFDS_H

#include <errno.h>
#include <stc/cstr.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "c_minilib_mock.h"

struct cmsu_PollCallback {
  int fd;
  void *data;
  cme_error_t (*callback)(const int revents, void *data);
};

#endif
