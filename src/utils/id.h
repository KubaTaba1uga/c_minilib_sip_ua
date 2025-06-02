/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_ID_H
#define C_MINILIB_SIP_UA_ID_H
#include <inttypes.h>
#include <stdint.h>

typedef uint32_t id_t;

typedef struct {
  id_t last_id;
} id_gen_t;

static inline id_t id_generate(id_gen_t *idgen) {
  idgen->last_id = (idgen->last_id + 1) % UINT32_MAX;

  return idgen->last_id;
}

#endif // C_MINILIB_SIP_UA_ID_H
