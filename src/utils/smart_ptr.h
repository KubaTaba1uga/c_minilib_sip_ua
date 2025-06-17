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
};

static inline void __my_generic_sp_destroy(struct my_generic_sp_value *sp) {
  sp->value_destroyh(sp->value);
};

static inline struct my_generic_sp_value
__my_generic_sp_clone(struct my_generic_sp_value sp) {
  return (struct my_generic_sp_value){.value = sp.value,
                                      .value_destroyh = sp.value_destroyh};
};

#define i_type my_generic_sp
#define i_key struct my_generic_sp_value
#define i_keydrop __my_generic_sp_destroy
#define i_keyclone __my_generic_sp_clone
#include "stc/arc.h"

static inline void *sp_alloc(uint32_t size, void (*value_destroyh)(void *)) {
  /*
    We are creating our custom smart pointer frame which looks sth like this:
     |                   |                            |       |
     | STC smart poniter | struct my_generic_sp_value | value |
     |                   |                            |       |

    Now we can use ptr to `value` part and go back to smart pointer part
    whenever needed.
  */
  void *value = malloc(sizeof(struct my_generic_sp) +
                       sizeof(struct my_generic_sp_value) + size);
  if (!value) {
    return NULL;
  }

  struct my_generic_sp *sp = value;

  *sp = my_generic_sp_make((struct my_generic_sp_value){
      .value = value, .value_destroyh = value_destroyh});

  struct my_generic_sp_value *sp_value = value + sizeof(struct my_generic_sp);

  sp_value->value_destroyh = value_destroyh;
  sp_value->value =
      value + sizeof(struct my_generic_sp) + sizeof(struct my_generic_sp_value);

  return sp_value->value;
};

static inline struct my_generic_sp *sp_get(void *value) {
  return value - sizeof(struct my_generic_sp) -
         sizeof(struct my_generic_sp_value);
};

static inline struct my_generic_sp_value *sp_value_get(void *value) {
  return value - sizeof(struct my_generic_sp_value);
};

static inline void *sp_ref(void *value) {
  struct my_generic_sp *sp = sp_get(value);

  my_generic_sp_clone(*sp);

  return value;
}

static inline void *sp_deref(void *value) {
  struct my_generic_sp *sp = sp_get(value);

  my_generic_sp_drop(sp);

  return value;
}

#endif
