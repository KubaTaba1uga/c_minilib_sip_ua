#ifndef C_MINILIB_SIP_UA_LINUX_POLL_H
#define C_MINILIB_SIP_UA_LINUX_POLL_H

#include <stc/cstr.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>

typedef struct pollfd cmsu_PollFd;

int cmsu_pollfd_cmp(cmsu_PollFd *a, cmsu_PollFd *b) { return a->fd > b->fd; }

cmsu_PollFd cmsu_pollfd_clone(cmsu_PollFd src) {
  return (struct pollfd){
      .fd = src.fd, .events = src.events, .revents = src.revents};
}

void cmsu_pollfd_drop(cmsu_PollFd *self) { close(self->fd); }

// Declare a managed, clonable vector of pollfd.
#define T cmsu_PollFds, cmsu_PollFd, (c_keyclass | c_use_cmp)
#include <stc/vec.h>

#endif
