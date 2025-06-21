/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UTILS_MEMORY_H
#define C_MINILIB_SIP_UA_UTILS_MEMORY_H
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"

static inline void *my_malloc(uint32_t size) {
  void *out = malloc(size);
  if (!out) {
    exit(ENOMEM);
  }

  return out;
}

static inline void *my_calloc(uint32_t amount, uint32_t size) {
  void *out = calloc(amount, size);
  if (!out) {
    exit(ENOMEM);
  }

  return out;
}

#endif // C_MINILIB_SIP_UA_UTILS_MEMORY_H
