/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_FD_HELPER_VEC_H
#define C_MINILIB_SIP_UA_INT_FD_HELPER_VEC_H

#include <stdint.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/_internal/common.h"
#include "event_loop/event_loop.h"

#define i_tag cmsu_FdHelpers
#define i_key uint32_t
#define i_val fd_helper_t
#include <stc/hmap.h>

static inline cme_error_t
my_hmap_cmsu_FdHelpers_insert(uint32_t fd, fd_helper_t helper,
                              struct hmap_cmsu_FdHelpers *helpers) {
  void *result = hmap_cmsu_FdHelpers_insert_or_assign(helpers, fd, helper).ref;
  cme_error_t err;

  if (!result) {
    err = cme_error(ENOMEM, "Cannot insert new fd helper to event loop");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline void my_hmap_cmsu_FdHelpers_remove(uint32_t fd,
                                                 hmap_cmsu_FdHelpers *helpers) {
  hmap_cmsu_FdHelpers_erase(helpers, fd);
}

static inline struct cmsu_FdHelper *
my_hmap_cmsu_FdHelpers_find(uint32_t fd, hmap_cmsu_FdHelpers *helpers) {
  return *hmap_cmsu_FdHelpers_at_mut(helpers, fd);
}

#endif
