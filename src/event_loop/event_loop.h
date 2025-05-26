#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_H

#include "c_minilib_error.h"
#include "event_loop/linux_poll.h"

struct cmsu_EventLoop {
  cmsu_PollFds poll_fds;
};

static inline cme_error_t
cmsu_event_loop_init(struct cmsu_EventLoop *event_loop) {
  event_loop->poll_fds = vec_cmsu_PollFds_init();
  return 0;
};

static inline cme_error_t
cmsu_event_loop_start(struct cmsu_EventLoop *event_loop) {

  return 0;
};

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
