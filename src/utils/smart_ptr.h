/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SMART_PTR_H
#define C_MINILIB_SIP_UA_SMART_PTR_H

/* #define SP_GET_VALUE(sp, name) (sp).get->name */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct __sp_payload {
  void *value;
  void (*value_destroyh)(void *);
  void *sp;
};

static inline struct my_generic_sp *
sp_get_from_handlers(struct __sp_payload *handlers);

static inline void __sp_payload_destroy(struct __sp_payload *sp_handler) {
  if (!sp_handler) {
    return;
  }

  if (sp_handler->value && sp_handler->value_destroyh) {
    sp_handler->value_destroyh(sp_handler->value);
  }
};

static inline struct __sp_payload __sp_payload_clone(struct __sp_payload data) {
  return data;
}

#define i_type my_generic_sp
#define i_key struct __sp_payload
#define i_keydrop __sp_payload_destroy
#define i_keyclone __sp_payload_clone
#include "stc/arc.h"

typedef void *(*__sp_alloch_t)(uint32_t);

static inline void *__sp_alloc(uint32_t size, void (*value_destroyh)(void *),
                               __sp_alloch_t alloc_fn) {
  /*
    We are creating our custom smart pointer frame which looks sth like this:
     |                   |        |                     |       |
     | STC smart poniter |   ->   | struct __sp_payload | value |
     |                   |        |                     |       |

    Now we can use ptr to `value` part and go back to smart pointer part
    whenever needed.
  */
  struct my_generic_sp *sp = alloc_fn(sizeof(struct my_generic_sp));
  if (!sp)
    return NULL;

  struct __sp_payload *pl = alloc_fn(sizeof(struct __sp_payload) + size);
  if (!pl) {
    free(sp);
    return NULL;
  }

  void *val = (unsigned char *)pl + sizeof(struct __sp_payload);

  *pl = (struct __sp_payload){
      .value = val,
      .value_destroyh = value_destroyh,
      .sp = sp,
  };
  *sp = my_generic_sp_from_ptr(pl);
  return val;
}

static inline void *sp_malloc(uint32_t size, void (*value_destroyh)(void *)) {
  return __sp_alloc(size, value_destroyh, (__sp_alloch_t)malloc);
}

static void *__sp_calloc_adapter(uint32_t n) { return calloc(1, n); }

static inline void *sp_zalloc(uint32_t size, void (*value_destroyh)(void *)) {
  return __sp_alloc(size, value_destroyh, __sp_calloc_adapter);
}

static inline struct my_generic_sp *sp_get(void *value) {
  return ((struct __sp_payload *)((unsigned char *)value -
                                  sizeof(struct __sp_payload)))
      ->sp;
};

static inline void *sp_ref(void *value) {
  struct my_generic_sp *sp = sp_get(value);

  my_generic_sp_clone(*sp);

  return value;
}

static inline void sp_deref(void *value) {
  struct my_generic_sp *sp = sp_get(value);
  uint32_t use_count = my_generic_sp_use_count(sp);

  my_generic_sp_drop(sp);

  if (--use_count == 0) {
    free(sp);
  }
}

#endif
