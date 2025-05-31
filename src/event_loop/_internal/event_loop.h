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

#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/poll_fd.h"
#include "event_loop/_internal/poll_fd_vec.h"
#include "socket/socket.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/

struct cmsu_EventLoop {
  cmsu_PollFds fds;
  vec_socket_t sockets;
};

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

cme_error_t cmsu_EventLoop_start(struct cmsu_EventLoop *evl) { return 0; };

void cmsu_EventLoop_destroy(struct cmsu_EventLoop **evl) {
  if (!evl || !*evl) {
    return;
  }

  vec_socket_destroy(&(*evl)->sockets);
  vec_cmsu_PollFds_drop(&(*evl)->fds);
  free(*evl);
  *evl = NULL;
};

cme_error_t cmsu_EventLoop_insert_socket(socket_t socket,
                                         struct cmsu_EventLoop *evl) {
  cmsu_PollFd *pollfd = vec_cmsu_PollFds_push(
      &evl->fds, (cmsu_PollFd){.fd = socket_get_fd(socket)});
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
