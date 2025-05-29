/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_INT_EVENT_LOOP_H

#include <asm-generic/errno.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
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
  cme_error_t err;
  err = cmsu_sock_list_create(&event_loop->sockets);
  if (err) {
    goto error_out;
  }

  event_loop->poll_fds = vec_cmsu_PollFds_init();

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t cmsu_EventLoop_insert_udp_socket(
    const char *ipaddr, uint32_t port, struct cmsu_SocketArg sockarg,
    cmsu_sock_t *out, struct cmsu_EventLoop *event_loop) {
  cme_error_t err;

  err = cmsu_sock_list_insert_udp(ipaddr, port, sockarg, out,
                                  event_loop->sockets);
  if (err) {
    goto error_out;
  }

  err = cmsu_PollFds_push(
      &event_loop->poll_fds,
      (cmsu_PollFd){.fd = cmsu_sock_get_fd(*out), .events = POLLIN});
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsu_EventLoop_pop_socket(int fd, struct cmsu_EventLoop *event_loop) {
  cmsu_sock_t socket;
  cme_error_t err;

  socket = cmsu_sock_list_find(fd, event_loop->sockets);
  if (!socket) {
    err = cme_error(ENODATA, "Cannot find socket");
    goto error_out;
  }

  vec_cmsu_PollFds_iter result =
      vec_cmsu_PollFds_find(&event_loop->poll_fds, (cmsu_PollFd){.fd = fd});

  vec_cmsu_PollFds_erase_at(&event_loop->poll_fds, result);

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsu_EventLoop_start(struct cmsu_EventLoop *event_loop) {
  cme_error_t err;

  while (true) {
    puts("Polling...");
    err = cmsu_PollFds_poll(&event_loop->poll_fds);
    if (err) {
      goto error_out;
    }

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
  cmsu_sock_list_destroy(&event_loop->sockets);
};

static inline cme_error_t
cmsu_EventLoop_process_events(struct cmsu_EventLoop *event_loop) {
  cmsu_sock_t socket;
  cme_error_t err;

  c_foreach(fd, vec_cmsu_PollFds, event_loop->poll_fds) {
    if (fd.ref->revents & (POLLIN | POLLOUT | POLLHUP | POLLERR)) {
      socket = cmsu_sock_list_find(fd.ref->fd, event_loop->sockets);
      assert(socket != NULL);

      if (fd.ref->revents & POLLIN) {
        err = cmsu_sock_recv(socket);
        if (err) {
          cmsu_EventLoop_pop_socket(fd.ref->fd, event_loop);
          goto error_out;
        }
      } else if (fd.ref->revents & POLLOUT) {
        err = cmsu_sock_send(socket);
        if (err) {
          cmsu_EventLoop_pop_socket(fd.ref->fd, event_loop);
          goto error_out;
        }
      } else {
        err = cme_error(ECONNABORTED,
                        "Error during sockets handling in event loop");
        cmsu_EventLoop_pop_socket(fd.ref->fd, event_loop);
        goto error_out;
      }
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_INTERNAL_H
