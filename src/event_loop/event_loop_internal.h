#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_INTERNAL_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_INTERNAL_H

#include "c_minilib_error.h"
#include "event_loop/poll/poll.h"
#include "event_loop/poll/pollfds.h"
#include "transp/transp.h"
#include "utils/error.h"

struct cmsu_EventLoop {
  cmsu_PollFds poll_fds; // We are using file descriptors to detect when user or
                         // another ua wants to comunicate with the app.
  // Timers
  // Sockets
};

static inline cme_error_t cmsu_evl_init(struct cmsu_EventLoop *event_loop) {
  event_loop->poll_fds = vec_cmsu_PollFds_init();
  return 0;
};

static inline cme_error_t cmsu_evl_start(struct cmsu_EventLoop *event_loop) {
  cmsu_UdpCtx ctx;
  cme_error_t err;

  // Create required file descriptor
  err = cmsu_udp_create(&ctx);
  if (err) {
    goto error_out;
  }

  // Push descriptor onto fds array.
  err = cmsu_PollFds_push(
      &event_loop->poll_fds,
      (cmsu_PollFd){.fd = ctx.sockfd, .events = POLLIN, .revents = 0});
  if (err) {
    goto error_out;
  }

  while (true) {
    err = cmsu_poll(&event_loop->poll_fds);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline void cmsu_evl_destroy(struct cmsu_EventLoop *event_loop) {
  vec_cmsu_PollFds_drop(&event_loop->poll_fds);
};

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_INTERNAL_H
