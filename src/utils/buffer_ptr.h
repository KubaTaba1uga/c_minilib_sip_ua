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

static inline void __csview_destroy(csview *view) { free((void *)view->buf); };
static inline csview __csview_clone(csview view) { return view; };

// TO-DO: use csview_drop and csview_clone
#define i_type BufferPtr
#define i_key struct csview
#define i_keydrop __csview_destroy
#define i_keyclone __csview_clone
#include "stc/arc.h"

static inline cme_error_t BufferPtr_create(uint32_t size,
                                           struct BufferPtr *out) {
  void *buf = calloc(size, sizeof(char));
  cme_error_t err;
  if (!buf) {
    err = cme_error(ENOMEM, "Cannot allocate memory for BufferPtr buffer");
    goto error_out;
  }

  *out = BufferPtr_from((csview){.buf = buf, .size = size});

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_UTILS_BUFFERPTR_H
