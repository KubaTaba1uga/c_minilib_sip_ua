/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_BYTE_BUF_PTR_H
#define C_MINILIB_SIP_UA_BYTE_BUF_PTR_H
#include <stdint.h>

#include "c_minilib_error.h"
#include "stc/cstr.h"
#include "stc/csview.h"

#include "smart_ptr.h"

typedef struct csview *byte_buf_t;

static inline cme_error_t byte_buf_create(uint32_t size, byte_buf_t *out) {
  unsigned char *buf = sp_zalloc(size + sizeof(struct csview), NULL);
  cme_error_t err;
  if (!buf) {
    err = cme_error(ENOMEM, "Cannot allocate memory for byte_buf_t");
    goto error_out;
  }

  struct csview *view = (struct csview *)buf;

  *view =
      c_sv((const char *)((unsigned char *)buf + sizeof(struct csview)), size);

  *out = view;

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_BYTE_BUF_PTR_H
