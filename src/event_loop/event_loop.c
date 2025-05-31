#include "event_loop/_internal/event_loop.h"
#include "event_loop/event_loop.h"

cme_error_t event_loop_create(event_loop_t *out) {
  return cmsu_EventLoop_create(out);
};
cme_error_t event_loop_start(event_loop_t evl) {
  return cmsu_EventLoop_start(evl);
};

void event_loop_destroy(event_loop_t *out) { cmsu_EventLoop_destroy(out); };
