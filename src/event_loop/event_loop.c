#include "event_loop/event_loop.h"
#include "event_loop/_internal/event_loop.h"
#include "event_loop/_internal/fd_vec.h"

cme_error_t event_loop_create(event_loop_t *out) {
  return __EventLoop_create(out);
};

cme_error_t event_loop_insert_socketfd(event_loop_t evlp, uint32_t fd,
                                       event_loop_sendh_t sendh,
                                       event_loop_recvh_t recvh, void *data) {
  return __EventLoop_insert_socketfd(evlp, fd, sendh, recvh, data);
}

void event_loop_remove_fd(event_loop_t evlp, int32_t fd) {
  __EventLoop_remove_fd(evlp, fd);
}

cme_error_t event_loop_set_pollin(event_loop_t evlp, int32_t fd) {
  return __PollFdsVec_set_pollin(fd, &evlp->get->fds);
};

cme_error_t event_loop_set_pollout(event_loop_t evlp, int32_t fd) {
  return __PollFdsVec_set_pollout(fd, &evlp->get->fds);
};

cme_error_t event_loop_unset_pollout(event_loop_t evlp, int32_t fd) {
  return __PollFdsVec_unset_pollout(fd, &evlp->get->fds);
};

cme_error_t event_loop_start(event_loop_t evlp) {
  return __EventLoop_start(evlp);
}

event_loop_t event_loop_ref(event_loop_t evlp) {
  return __EventLoop_ref(evlp);
};

void event_loop_deref(event_loop_t evlp) { __EventLoop_deref(evlp); };
