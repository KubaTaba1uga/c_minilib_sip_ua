/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_FD_HELPER_VEC_H
#define C_MINILIB_SIP_UA_INT_FD_HELPER_VEC_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/_internal/fd_helper.h"
#include "event_loop/event_loop.h"
#include <stdint.h>

#define i_tag cmsu_FdHelpers
#define i_key struct cmsu_FdHelper
#include <stc/vec.h>

cme_error_t my_vec_cmsu_FdHelpers_insert(struct vec_cmsu_FdHelpers *helpers,
                                         uint32_t fd, event_loop_sendh_t sendh,
                                         event_loop_recvh_t recvh, void *data) {
  struct cmsu_FdHelper fd_helper = {
      .sendh = sendh, .recvh = recvh, .data = data};

  if ((size_t)fd >= helpers->size) {
    bool resized = vec_cmsu_FdHelpers_resize(helpers, fd + 1, fd_helper);
    if (!resized) {
      return cme_return(cme_error(ENOMEM, "Unable to resize `helpers`"));
    }
  } else {
    struct cmsu_FdHelper *result =
        vec_cmsu_FdHelpers_insert_n(helpers, fd, &fd_helper, 1).ref;
    if (!result) {
      return cme_return(
          cme_error(ENOMEM, "Unable to add new socket to `helpers`"));
    }
  }

  return 0;
}

static inline void my_vec_cmsu_FdHelpers_remove(uint32_t fd,
                                                vec_cmsu_FdHelpers *helpers) {
  vec_cmsu_FdHelpers_erase_n(helpers, fd, 1);
}

#endif
