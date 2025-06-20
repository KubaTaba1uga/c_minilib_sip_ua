#include "timer_fd/timer_fd.h"
#include "event_loop/event_loop.h"
#include "timer_fd/_internal/timer_fd.h"

cme_error_t timer_fd_create(event_loop_t evl, time_t seconds, long nseconds,
                            timer_fd_timeouth_t timeouth, void *timeouth_arg,
                            timer_fd_t *out) {
  return __Timer_create(evl, seconds, nseconds, timeouth, timeouth_arg, out);
};

timer_fd_t timer_fd_ref(timer_fd_t timerp) { return __TimerFd_ref(timerp); };

void timer_fd_deref(timer_fd_t timerp) { __TimerFd_deref(timerp); };
