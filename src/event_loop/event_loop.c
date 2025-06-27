#include "event_loop/event_loop.h"
#include "event_loop/_internal/event_loop.h"
#include "event_loop/_internal/fd_helper.h"
#include "event_loop/_internal/fd_helper_hmap.h"
#include "utils/generic_ptr.h"
#include "utils/memory.h"

#include <stdio.h>
#include <string.h>

static inline cme_error_t __EventLoop_process_events(struct EventLoopPtr evlp);

cme_error_t EventLoopPtr_create(struct EventLoopPtr *out) {
  struct __EventLoop *evl = my_malloc(sizeof(struct __EventLoop));

  *evl = (struct __EventLoop){.fds = vec__PollFdsVec_init(),
                              .fds_helpers = hmap__FdHelpersMap_init()};

  *out = EventLoopPtr_from_ptr(evl);

  return 0;
};

void __EventLoop_destroy(struct __EventLoop *evl) {
  puts(__func__);
  vec__PollFdsVec_drop(&evl->fds);
  hmap__FdHelpersMap_drop(&evl->fds_helpers);
};

cme_error_t EventLoopPtr_insert_socketfd(struct EventLoopPtr evlp, uint32_t fd,
                                         event_loop_recvh_t recvh,
                                         struct GenericPtr data) {
  puts(__func__);
  cme_error_t err;
  err = __PollFdsVec_push(&evlp.get->fds, (__PollFd){.fd = fd});
  if (err) {
    goto error_out;
  }

  err = __FdHelpersMap_insert(fd,
                              (__FdHelper){.fd_type = __FdType_SOCKET,
                                           .timeouth = NULL,
                                           .recvh = recvh,
                                           .data = data},
                              &evlp.get->fds_helpers);
  if (err) {
    goto error_fds_cleanup;
  }

  return 0;

error_fds_cleanup:
  __PollFdsVec_remove(fd, &evlp.get->fds);
error_out:
  return cme_return(err);
}

cme_error_t EventLoopPtr_insert_timerfd(struct EventLoopPtr evlp, uint32_t fd,
                                        event_loop_timeouth_t timeouth,
                                        struct GenericPtr data) {
  cme_error_t err;

  err = __PollFdsVec_push(&evlp.get->fds,
                          (__PollFd){.fd = fd, .events = 0, .revents = 0});
  if (err) {
    goto error_out;
  }

  err = __FdHelpersMap_insert(fd,
                              (__FdHelper){.fd_type = __FdType_TIMER,
                                           .timeouth = timeouth,
                                           .recvh = NULL,
                                           .data = data},
                              &evlp.get->fds_helpers);
  if (err) {
    goto error_fds_cleanup;
  }

  return 0;

error_fds_cleanup:
  __PollFdsVec_remove(fd, &evlp.get->fds);
error_out:
  return cme_return(err);
}

struct GenericPtr *EventLoopPtr_get_fd_arg(struct EventLoopPtr evlp,
                                           uint32_t fd,
                                           struct GenericPtr *out) {
  struct __FdHelper *helper = __FdHelpersMap_find(fd, &evlp.get->fds_helpers);
  if (!helper) {
    return NULL;
  }

  *out = helper->data;

  return out;
}

void EventLoopPtr_remove_fd(struct EventLoopPtr evlp, int32_t fd) {
  __FdHelpersMap_remove(fd, &evlp.get->fds_helpers);
  __PollFdsVec_remove(fd, &evlp.get->fds);
}

cme_error_t EventLoopPtr_set_pollin(struct EventLoopPtr evlp, int32_t fd) {
  return __PollFdsVec_set_pollin(fd, &evlp.get->fds);
}

cme_error_t EventLoopPtr_start(struct EventLoopPtr evlp) {
  puts(__func__);
  cme_error_t err;

  // TO-DO delete this dummy destroy mechanism
  // TO-DO design mechanism to stop event loop
  //       and safely cleanup all it's resources.
  //       Something like QUIT command for evl.
  int i = 0;
  while (true && i++ < 3) {
    err = __PollFdsVec_poll(&evlp.get->fds);
    if (err) {
      goto error_out;
    }

    err = __EventLoop_process_events(evlp);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t __EventLoop_process_events(struct EventLoopPtr evlp) {
  puts(__func__);
  cme_error_t err;

  c_foreach(fd, vec__PollFdsVec, evlp.get->fds) {
    __FdHelper *fd_helper =
        __FdHelpersMap_find(fd.ref->fd, &evlp.get->fds_helpers);

    assert(fd_helper != NULL);

    if (fd.ref->revents & POLLIN) {
      err = __FdHelper_pollinh(fd_helper);
      if (err) {
        goto error_out;
      }
    }
  }

  return 0;

error_out:
  return cme_return(err);
}
