/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SMART_PTR_H
#define C_MINILIB_SIP_UA_SMART_PTR_H

/* #define SP_GET_VALUE(sp, name) (sp).get->name */
#include <stdint.h>
#include <stdlib.h>

#define SP_UNWRAP(sp) (*(sp).get)
#define SP_GET_PTR(sp, name) &(*(sp).get)->name
#define SP_SET_VALUE(sp, name, value) (*(sp).get)->name = value
typedef struct my_generic_sp my_generic_sp;

struct my_generic_sp_value {
  void *value;
  void (*value_destroyh)(void *);
  void *(*value_cloneh)(void *);
};

static inline void __my_generic_sp_destroy(struct my_generic_sp_value *sp) {
  sp->value_destroyh(sp->value);
};

static inline struct my_generic_sp_value
__my_generic_sp_clone(struct my_generic_sp_value sp) {
  return (struct my_generic_sp_value){.value = sp.value_cloneh(sp.value),
                                      .value_cloneh = sp.value_cloneh,
                                      .value_destroyh = sp.value_destroyh};
};

#define i_type my_generic_sp
#define i_key struct my_generic_sp_value
#define i_keydrop __my_generic_sp_destroy
#define i_keyclone __my_generic_sp_clone
#include "stc/arc.h"

static inline void *sp_alloc(uint32_t size, void (*value_destroyh)(void *),
                             void *(*value_cloneh)(void *)) {
  void *value = malloc(size + sizeof(struct my_generic_sp_value) +
                       sizeof(struct my_generic_sp));
  if (!value) {
    return NULL;
  }

  /* struct my_generic_sp *sp = value; */

  struct my_generic_sp sp = my_generic_sp_make(
      (struct my_generic_sp_value){.value = value,
                                   .value_cloneh = value_cloneh,
                                   .value_destroyh = value_destroyh});
  (void)sp;

  return 0;
};

#endif
