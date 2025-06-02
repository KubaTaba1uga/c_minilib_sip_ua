/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SOCKET_VEC_H
#define C_MINILIB_SIP_UA_SOCKET_VEC_H

#include "c_minilib_error.h"
#include "socket/_internal/socket.h"
#include "socket/socket.h"
#include "stc/common.h"
#include "utils/common.h"
#include <stdint.h>
#include <stdlib.h>

#define i_tag cmsu_Sockets
#define i_key struct cmsu_Socket
#define i_cmp cmsu_Socket_cmp
#include <stc/vec.h>

static inline cme_error_t cmsu_Sockets_create(vec_socket_t *out) {
  struct vec_cmsu_Sockets *socket_vec = malloc(sizeof(struct vec_cmsu_Sockets));
  if (!socket_vec) {
    return cme_return(
        cme_error(ENOMEM, "Cannot allocate memory for `sockets`"));
  }

  *socket_vec = vec_cmsu_Sockets_init();
  *out = socket_vec;

  return 0;
};

static inline cme_error_t cmsu_Sockets_insert(socket_t socket,
                                              vec_cmsu_Sockets *sockets) {
  int32_t fd = cmsu_Socket_get_fd(socket);
  if (fd < 0) {
    return cme_return(
        cme_error(ENOMEM, "No file descriptor assigned to `socket`"));
  }

  if ((size_t)fd >= sockets->size) {
    bool resized = vec_cmsu_Sockets_resize(
        sockets, fd + 1, *socket); // or reserve + manual insert
    if (!resized) {
      return cme_return(cme_error(ENOMEM, "Unable to resize `sockets`"));
    }
  } else {
    socket_t result = vec_cmsu_Sockets_insert_n(sockets, fd, socket, 1).ref;
    if (!result) {
      return cme_return(
          cme_error(ENOMEM, "Unable to add new socket to `sockets`"));
    }
  }

  return 0;
}

static inline socket_t cmsu_Sockets_get(uint32_t fd,
                                        vec_cmsu_Sockets *sockets) {
  socket_t result = (socket_t)vec_cmsu_Sockets_at(sockets, fd);
  if (!result) {
    return NULL;
  }

  return result;
}

static inline void cmsu_Sockets_destroy(vec_socket_t *out) {
  if (!out || !*out) {
    return;
  }

  vec_cmsu_Sockets_drop(*out);
  free(*out);
  *out = NULL;
}

static inline void cmsu_Sockets_remove(uint32_t fd, vec_cmsu_Sockets *sockets) {
  vec_cmsu_Sockets_erase_n(sockets, fd, 1);
}

#endif
