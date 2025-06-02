/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
/*
  This is interface to int_event_loop module, if you need anything from
  int_event_loop module, put interface declaration here and interface
  implementation in .c but always write real implementation as static inline in
  _internal.
 */

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/poll_fd.h"
#include "event_loop/_internal/poll_fd_vec.h"
#include "socket/socket.h"
#include "stc/common.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/

struct cmsu_EventLoop {
  cmsu_PollFds fds;
  vec_socket_t sockets;
};

static inline cme_error_t
cmsu_EventLoop_process_events(struct cmsu_EventLoop *evl);

static inline cme_error_t cmsu_EventLoop_create(struct cmsu_EventLoop **out) {
  struct cmsu_EventLoop *evl = calloc(1, sizeof(struct cmsu_EventLoop));
  cme_error_t err;
  if (!evl) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `evl`");
    goto error_out;
  }

  evl->fds = vec_cmsu_PollFds_init();
  err = vec_socket_create(&evl->sockets);
  if (err) {
    goto error_evl_cleanup;
  }

  *out = evl;

  return 0;

error_evl_cleanup:
  free(evl);
error_out:
  return cme_return(err);
};

cme_error_t cmsu_EventLoop_start(struct cmsu_EventLoop *evl) {
  cme_error_t err;
  int poll_res;

  while (true) {
    poll_res = poll(evl->fds.data, evl->fds.size, -1);
    if (poll_res < 0) {
      err = cme_error(EIO, "Polling error");
      goto error_out;
    }

    err = cmsu_EventLoop_process_events(evl);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
};

void cmsu_EventLoop_destroy(struct cmsu_EventLoop **evl) {
  if (!evl || !*evl) {
    return;
  }

  vec_socket_destroy(&(*evl)->sockets);
  vec_cmsu_PollFds_drop(&(*evl)->fds);
  free(*evl);
  *evl = NULL;
};

cme_error_t cmsu_EventLoop_insert_socket(socket_t socket, short events,
                                         struct cmsu_EventLoop *evl) {
  cmsu_PollFd *pollfd = vec_cmsu_PollFds_push(
      &evl->fds, (cmsu_PollFd){.fd = socket_get_fd(socket), .events = events});
  cme_error_t err;
  if (!pollfd) {
    err = cme_error(ENOMEM, "Cannot add `pollfd` to event loop fds");
    goto error_out;
  }

  err = vec_socket_insert(socket, evl->sockets);
  if (err) {
    goto error_fds_cleanup;
  }

  return 0;

error_fds_cleanup:
  vec_cmsu_PollFds_pop(&evl->fds);
error_out:
  return cme_return(err);
}

cme_error_t cmsu_EventLoop_process_events(struct cmsu_EventLoop *evl) {
  socket_t socket;
  cme_error_t err;

  c_foreach(fd, vec_cmsu_PollFds, evl->fds) {
    socket = vec_socket_get(fd.ref->fd, evl->sockets);
    if (!socket) {
      err = cme_error(ENODATA, "Cannot find socket matching file descriptor");
      goto error_out;
    }

    err = 0;
    if (fd.ref->revents & POLLIN) {
      err = socket_recv_event_handler(socket);
      if (err) {
        goto error_out;
      }
    }
    if (fd.ref->revents & POLLOUT) {
      bool is_send_done = true;
      err = socket_send_event_handler(socket, &is_send_done);
      if (err) {
        goto error_out;
      }
      if (is_send_done) {
        // Mark send as completed
        fd.ref->events &= ~POLLOUT;
      }
    }
    if (fd.ref->revents & (POLLERR)) {
      err = socket_fail_event_handler(true, false, socket);
      if (err) {
        goto error_out;
      }
    }
    if (fd.ref->revents & (POLLHUP)) {
      err = socket_fail_event_handler(false, true, socket);
      if (err) {
        goto error_out;
      }
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

void cmsu_EventLoop_async_send_socket(socket_t socket,
                                      struct cmsu_EventLoop *evl) {
  c_foreach(fd, vec_cmsu_PollFds, evl->fds) {
    if (socket_get_fd(socket) == fd.ref->fd) {
      fd.ref->events |= POLLOUT;
      break;
    }
  }
}

void cmsu_EventLoop_remove_socket(socket_t socket, struct cmsu_EventLoop *evl) {
  cmsu_PollFd *pollfd = cmsu_poll(evl->fds, ) cmsu_PollFd *pollfd =
      vec_cmsu_PollFds_(&evl->fds, (cmsu_PollFd){.fd = socket_get_fd(socket),
                                                 .events = events});
  cme_error_t err;
  if (!pollfd) {
    err = cme_error(ENOMEM, "Cannot add `pollfd` to event loop fds");
    goto error_out;
  }

  err = vec_socket_insert(socket, evl->sockets);
  if (err) {
    goto error_fds_cleanup;
  }

  return 0;

error_fds_cleanup:
  vec_cmsu_PollFds_pop(&evl->fds);
error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_INT_EVENT_LOOP_H
