#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_H

#include "c_minilib_error.h"

struct cmsc_EventLoop {};

static inline cme_error_t
cmsu_event_loop_init(struct cmsc_EventLoop *event_loop) {
  return 0;
};

static inline cme_error_t
cmsu_event_loop_start(struct cmsc_EventLoop *event_loop) {
  return 0;
};

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
