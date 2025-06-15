#include "event_loop/event_loop.h"
#include "event_loop/_internal/event_loop.h"

cme_error_t event_loop_create(event_loop_t *out) {
  return __EventLoop_create(out);
};
