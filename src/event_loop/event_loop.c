#include <stdint.h>

#include "event_loop/_internal/event_loop.h"
#include "event_loop/_internal/fd_vec.h"
#include "event_loop/event_loop.h"
#include "stc/common.h"

cme_error_t event_loop_create(event_loop_t *out) {
  return cmsu_EventLoop_create(out);
};
cme_error_t event_loop_start(event_loop_t evl) {
  return cmsu_EventLoop_start(evl);
};

void event_loop_destroy(event_loop_t *out) { cmsu_EventLoop_destroy(out); };

cme_error_t event_loop_insert_fd(event_loop_t evl, fd_t fd,
                                 event_loop_sendh_t sendh,
                                 event_loop_recvh_t recvh, void *data) {
  return cmsu_EventLoop_insert_fd(evl, fd, sendh, recvh, data);
}

void event_loop_remove_fd(event_loop_t evl, int32_t fd) {
  cmsu_EventLoop_remove_fd(evl, fd);
}

cme_error_t event_loop_fd_set_pollout(event_loop_t evl, int32_t fd) {
  return my_vec_cmsu_Fds_set_pollout(fd, &evl->fds);
};
