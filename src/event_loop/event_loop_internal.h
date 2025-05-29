#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_INTERNAL_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_INTERNAL_H

#include <asm-generic/errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "event_loop/poll/poll.h"
#include "event_loop/poll/pollfds.h"
#include "transp/transp.h"
#include "transp/udp.h"
#include "utils/error.h"

struct cmsu_EventLoop {
  cmsu_PollFds poll_fds; // We are using file descriptors to detect when user or
                         // another ua wants to comunicate with the app.

  cmsu_Transports
      transps; // All fds from poll_fds are bounded to transp object or to timer
               // object. Transp is used for external comunnication,
               // timers are used for internal communication. Timer
               // object is not implemented yet.
};

static inline cme_error_t
cmsu_evl_process_events(struct cmsu_EventLoop *event_loop);

static inline cme_error_t cmsu_evl_init(struct cmsu_EventLoop *event_loop) {
  event_loop->poll_fds = vec_cmsu_PollFds_init();
  event_loop->transps = list_cmsu_Transports_init();

  return 0;
};

static inline cme_error_t
cmsu_evl_insert_udp_transp(short revents, struct cmsu_TransportUdp *transp,
                           struct cmsu_EventLoop *event_loop) {
  cme_error_t err;

  // Insert UDP transport object into the event loop.
  err = cmsu_Transports_insert_udp(transp, &event_loop->transps);
  if (err) {
    goto error_out;
  }

  // Push file descriptor onto poll_fds array.
  err =
      cmsu_PollFds_push(&event_loop->poll_fds,
                        (cmsu_PollFd){.fd = transp->sockfd, .events = revents});
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t cmsu_evl_start(struct cmsu_EventLoop *event_loop) {
  cme_error_t err;

  while (true) {
    puts("Polling...");
    err = cmsu_poll(&event_loop->poll_fds);
    if (err) {
      goto error_out;
    }

    err = cmsu_evl_process_events(event_loop);
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

static inline cme_error_t
cmsu_evl_process_events(struct cmsu_EventLoop *event_loop) {
  cme_error_t err;

  c_foreach(fd, vec_cmsu_PollFds, event_loop->poll_fds) {
    if (fd.ref->revents & (POLLHUP | POLLOUT | POLLIN | POLLERR)) {
      // We are making assumption that there is no PollFd without TransportCtx.
      cmsu_Transport *transp =
          cmsu_Transports_find(fd.ref->fd, event_loop->transps);
      assert(transp != NULL);

      err = transp->process_events_func(fd.ref, transp->ctx);
      if (err) {
        goto error_out;
      }
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_INTERNAL_H
