#include "event_loop/event_loop.h"
#include "event_loop/_internal/event_loop.h"
#include "event_loop/_internal/fd_vec.h"

cme_error_t event_loop_create(event_loop_ptr_t *out) {
  return __EventLoop_create(out);
};

event_loop_ptr_t event_loop_ref(event_loop_ptr_t evl) {
  return __EventLoop_ref(evl);
}

void event_loop_deref(event_loop_ptr_t *evl) { __EventLoop_deref(evl); }

cme_error_t event_loop_insert_socketfd(event_loop_ptr_t evl, uint32_t fd,
                                       event_loop_sendh_t sendh,
                                       event_loop_recvh_t recvh, void *data) {
  return __EventLoop_insert_socketfd(evl, fd, sendh, recvh, data);
}

void event_loop_remove_fd(event_loop_ptr_t evl, int32_t fd) {
  __EventLoop_remove_fd(evl, fd);
}

cme_error_t event_loop_set_pollin(event_loop_ptr_t evl, int32_t fd) {
  return __PollFdsVec_set_pollin(fd, SP_GET_PTR(evl, fds));
};

cme_error_t event_loop_set_pollout(event_loop_ptr_t evl, int32_t fd) {
  return __PollFdsVec_set_pollout(fd, SP_GET_PTR(evl, fds));
};

cme_error_t event_loop_unset_pollout(event_loop_ptr_t evl, int32_t fd) {
  return __PollFdsVec_unset_pollout(fd, SP_GET_PTR(evl, fds));
};

cme_error_t event_loop_start(event_loop_ptr_t evl_ptr) {
  return __EventLoop_start(evl_ptr);
}
