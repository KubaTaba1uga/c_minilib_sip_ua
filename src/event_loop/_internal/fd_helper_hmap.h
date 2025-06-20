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
#include "event_loop/_internal/fd_helper.h"

#define i_tag _FdHelpersMap
#define i_key uint32_t
#define i_val __FdHelper
#include <stc/hmap.h>

typedef struct hmap__FdHelpersMap __FdHelpersMap;

static inline cme_error_t __FdHelpersMap_insert(uint32_t fd, __FdHelper helper,
                                                __FdHelpersMap *helpers) {
  void *result = hmap__FdHelpersMap_insert_or_assign(helpers, fd, helper).ref;
  cme_error_t err;

  if (!result) {
    err = cme_error(ENOMEM, "Cannot insert new fd helper to event loop");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline void __FdHelpersMap_remove(uint32_t fd, __FdHelpersMap *helpers) {
  hmap__FdHelpersMap_erase(helpers, fd);
}

static inline struct __FdHelper *
__FdHelpersMap_find(uint32_t fd, hmap__FdHelpersMap *helpers) {
  return hmap__FdHelpersMap_at_mut(helpers, fd);
}

#endif
