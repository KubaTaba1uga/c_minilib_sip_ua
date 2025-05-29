#ifndef C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_INT_EVENT_LOOP_H

#include <errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/poll_fd.h"
#include "event_loop/_internal/poll_fd_vec.h"
#include "socket/socket.h"
#include "utils/error.h"

struct cmsu_EventLoop {
  cmsu_PollFds poll_fds; // We are using file descriptors to detect when user or
                         // another ua wants to comunicate with the app.

  cmsu_sock_list_t
      sockets; // Every new fd is bounded to socket object or
               // to timer object. Socket is used for external
               // comunnication, timer is used for internal
               // communication. Timer object is not implemented yet.
};

static inline cme_error_t
cmsu_EventLoop_process_events(struct cmsu_EventLoop *event_loop);

static inline cme_error_t
cmsu_EventLoop_init(struct cmsu_EventLoop *event_loop) {
  event_loop->poll_fds = vec_cmsu_PollFds_init();
  /* event_loop->sockets = list_cmsu_Sockets_init(); */

  return 0;
};

static inline cme_error_t
cmsu_EventLoop_insert_udp_transp(short revents, struct cmsu_SocketUdp *transp,
                                 struct cmsu_EventLoop *event_loop) {
  /* cme_error_t err; */

  // Insert UDP socket object into the event loop.
  /* err = cmsu_Sockets(transp, &event_loop->transps); */
  /* if (err) { */
  /*   goto error_out; */
  /* } */

  // Push file descriptor onto poll_fds array.
  /* err = */
  /*     cmsu_PollFds_push(&event_loop->poll_fds, */
  /*                       (cmsu_PollFd){.fd = transp->sockfd, .events =
   * revents}); */
  /* if (err) { */
  /*   goto error_out; */
  /* } */

  return 0;

  /* error_out: */
  /*   return cme_return(err); */
};

static inline cme_error_t
cmsu_EventLoop_start(struct cmsu_EventLoop *event_loop) {
  cme_error_t err;

  while (true) {
    puts("Polling...");
    /* err = cmsu_poll(&event_loop->poll_fds); */
    /* if (err) { */
    /*   goto error_out; */
    /* } */

    err = cmsu_EventLoop_process_events(event_loop);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline void cmsu_EventLoop_destroy(struct cmsu_EventLoop *event_loop) {
  vec_cmsu_PollFds_drop(&event_loop->poll_fds);
};

static inline cme_error_t
cmsu_EventLoop_process_events(struct cmsu_EventLoop *event_loop) {
  /* cme_error_t err; */

  c_foreach(fd, vec_cmsu_PollFds, event_loop->poll_fds) {
    if (fd.ref->revents & (POLLHUP | POLLOUT | POLLIN | POLLERR)) {
      // We are making assumption that there is no PollFd without SocketCtx.
      /* cmsu_Socket *transp = cmsu_Sockets_find(fd.ref->fd,
       * event_loop->transps); */
      /* assert(transp != NULL); */

      /* err = transp->process_events_func(fd.ref, transp->ctx); */
      /* if (err) { */
      /*   goto error_out; */
      /* } */
    }
  }

  return 0;

  /* error_out: */
  /*   return cme_return(err); */
}

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_INTERNAL_H
