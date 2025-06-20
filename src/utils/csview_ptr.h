/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UTILS_CSVIEW_PTR_H
#define C_MINILIB_SIP_UA_UTILS_CSVIEW_PTR_H
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "stc/cstr.h"
#include "stc/csview.h"

static inline void __csview_destroy(csview *view) { free((void *)view->buf); };
static inline csview __csview_clone(csview view) { return view; };

#define i_type __csviewPtr
#define i_key csview
#define i_keydrop __csview_destroy
#define i_keyclone __csview_clone
#include "stc/arc.h"

typedef struct __csviewPtr csview_ptr_t;

static inline cme_error_t csview_ptr_create(uint32_t size, csview_ptr_t *out) {
  void *buf = calloc(size, sizeof(char));
  cme_error_t err;
  if (!buf) {
    err = cme_error(ENOMEM, "Cannot allocate memory for csview_ptr buffer");
    goto error_out;
  }

  *out = __csviewPtr_from((csview){.buf = buf, .size = size});

  return 0;

error_out:
  return cme_return(err);
}

static inline csview_ptr_t csview_ptr_ref(csview_ptr_t csview_ptrp) {
  return __csviewPtr_clone(csview_ptrp);
};

static inline void csview_ptr_deref(csview_ptr_t csview_ptrp) {
  __csviewPtr_drop(&csview_ptrp);
};

static inline csview_ptr_t csview_ptr_from(struct csview buffer) {
  return __csviewPtr_from(buffer);
};

#endif // C_MINILIB_SIP_UA_UTILS_CSVIEW_PTR_H
