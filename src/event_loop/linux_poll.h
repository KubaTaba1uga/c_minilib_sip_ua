#ifndef C_MINILIB_SIP_UA_LINUX_POLL_H
#define C_MINILIB_SIP_UA_LINUX_POLL_H

#include <stc/cstr.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>

typedef struct pollfd cmsu_PollFd;

static inline int cmsu_PollFd_cmp(const cmsu_PollFd *a, const cmsu_PollFd *b) {
  return a->fd > b->fd;
}

static inline cmsu_PollFd cmsu_PollFd_clone(cmsu_PollFd src) {
  return (struct pollfd){
      .fd = src.fd, .events = src.events, .revents = src.revents};
}

static inline void cmsu_PollFd_drop(cmsu_PollFd *self) { close(self->fd); }

// STC configuration:
#define i_tag cmsu_PollFds
#define i_keyclass cmsu_PollFd
#define i_cmp cmsu_PollFd_cmp
#include <stc/vec.h>

#endif
