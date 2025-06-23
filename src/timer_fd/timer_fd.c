#include "timer_fd/timer_fd.h"
#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "timer_fd/_internal/timer_fd.h"
#include "utils/generic_ptr.h"

static inline cme_error_t __Timer_timeouth(struct GenericPtr data);

cme_error_t TimerFdPtr_create(struct EventLoopPtr evl, time_t seconds,
                              long nseconds, timer_fd_timeouth_t timeouth,
                              struct GenericPtr timeouth_arg,
                              struct TimerFdPtr *out) {
  cme_error_t err;

  errno = 0;
  int timerfd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
  if (timerfd == -1) {
    err = cme_error(errno, "OS cannot create timer");
    goto error_out;
  }

  struct itimerspec clock_value = {0};
  clock_value.it_value.tv_sec = seconds;
  clock_value.it_value.tv_nsec = nseconds;

  errno = 0;
  if (timerfd_settime(timerfd, 0, &clock_value, NULL) != 0) {
    err = cme_error(errno, "OS cannot configure timer");
    goto error_timerfd_cleanup;
  };

  err = EventLoopPtr_set_pollin(evl, timerfd);
  if (err) {
    err = cme_errorf(errno, "Cannot set POLLIN on timer in event loop");
    goto error_evl_cleanup;
  }

  *out = TimerFdPtr_from((struct __TimerFd){
      .evl = EventLoopPtr_clone(evl),
      .timeouth = timeouth,
      .timeouth_arg = timeouth_arg,
      .fd = timerfd,
  });

  err = EventLoopPtr_insert_timerfd(evl, timerfd, __Timer_timeouth,
                                    GenericPtr_from_arc(TimerFdPtr, out));
  if (err) {
    TimerFdPtr_drop(out);
    err = cme_errorf(errno, "Cannot insert timer into event loop");
    goto error_timerptr_cleanup;
  }

  return 0;

error_timerptr_cleanup:
  TimerFdPtr_drop(out);
error_evl_cleanup:
  EventLoopPtr_remove_fd(evl, timerfd);
error_timerfd_cleanup:
  close(timerfd);
error_out:
  return cme_return(err);
};

static inline cme_error_t __Timer_timeouth(struct GenericPtr data) {
  struct TimerFdPtr timer = GenericPtr_dump(TimerFdPtr, data);
  cme_error_t err;

  timer.get->do_cleanup = true;

  err = timer.get->timeouth(timer, timer.get->timeouth_arg);
  if (err || timer.get->do_cleanup) {
    EventLoopPtr_remove_fd(timer.get->evl, timer.get->fd);
    close(timer.get->fd);
    TimerFdPtr_drop(&timer);
  }

  return err;
};

void TimerFdPtr_rearm(struct TimerFdPtr timerp) {
  timerp.get->do_cleanup = false;
};

void __TimerFd_destroy(struct __TimerFd *timerp) {
  EventLoopPtr_drop(&timerp->evl);
};

struct __TimerFd __TimerFd_clone(struct __TimerFd timerp) { return timerp; };
