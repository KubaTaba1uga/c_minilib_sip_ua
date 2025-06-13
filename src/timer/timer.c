#include "timer/timer.h"
#include "timer/_internal/timer.h"

cme_error_t mytimer_create(event_loop_t evl, time_t seconds, long nseconds,
                           timer_timeouth_t timeouth, void *timeouth_arg,
                           mytimer_t *out) {

  return cmsu_Timer_create(evl, seconds, nseconds, timeouth, timeouth_arg, out);
};
