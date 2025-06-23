/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UTILS_BUFFERPTR_H
#define C_MINILIB_SIP_UA_UTILS_BUFFERPTR_H
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "stc/cstr.h"
#include "stc/csview.h"
#include "utils/memory.h"

static inline void __csview_destroy(csview *view) { free((void *)view->buf); };
static inline csview __csview_clone(csview view) { return view; };

#define i_type BufferPtr
#define i_key struct csview
#define i_keydrop __csview_destroy
#define i_keyclone __csview_clone
#include "stc/arc.h"

static inline cme_error_t BufferPtr_create_empty(uint32_t size,
                                                 struct BufferPtr *out) {
  void *buf = my_calloc(size, sizeof(char));
  struct csview *view = my_malloc(sizeof(struct csview));

  *view = (struct csview){.buf = buf, .size = size};

  *out = BufferPtr_from_ptr(view);

  return 0;
}

static inline cme_error_t BufferPtr_create_filled(struct csview view,
                                                  struct BufferPtr *out) {
  struct csview *new_view = my_malloc(sizeof(struct csview));

  *new_view = (struct csview){.buf = view.buf, .size = view.size};

  *out = BufferPtr_from_ptr(new_view);

  return 0;
}

#undef BufferPtr_from
#undef BufferPtr_from_ptr

#endif // C_MINILIB_SIP_UA_UTILS_BUFFERPTR_H
