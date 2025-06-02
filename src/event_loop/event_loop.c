#include "event_loop/_internal/event_loop.h"
#include "event_loop/event_loop.h"
#include "socket/socket.h"

cme_error_t event_loop_create(event_loop_t *out) {
  return cmsu_EventLoop_create(out);
};
cme_error_t event_loop_start(event_loop_t evl) {
  return cmsu_EventLoop_start(evl);
};

void event_loop_destroy(event_loop_t *out) { cmsu_EventLoop_destroy(out); };

cme_error_t event_loop_insert_socket(socket_t socket, short events,
                                     event_loop_t evl) {
  return cmsu_EventLoop_insert_socket(socket, events, evl);
};

void event_loop_async_send_socket(socket_t socket) {
  cmsu_EventLoop_async_send_socket(socket);
};

void event_loop_remove_socket(socket_t socket) {
  cmsu_EventLoop_remove_socket(socket);
};
