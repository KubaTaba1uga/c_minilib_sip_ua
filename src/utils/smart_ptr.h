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
struct __sp_handlers {
  void *value;
  void (*value_destroyh)(void *);
};

static inline struct my_generic_sp *
sp_get_from_handlers(struct __sp_handlers *handlers);

static inline void __sp_handlers_destroy(struct __sp_handlers *sp_handler) {
  if (!sp_handler) {
    return;
  }

  if (sp_handler->value && sp_handler->value_destroyh) {
    sp_handler->value_destroyh(sp_handler->value);
  }

  void *sp = sp_get_from_handlers(sp_handler);

  free(sp);
};

static inline struct __sp_handlers
__sp_handlers_clone(struct __sp_handlers sp) {
  return (struct __sp_handlers){.value = sp.value,
                                .value_destroyh = sp.value_destroyh};
};

#define i_type my_generic_sp
#define i_key struct __sp_handlers
#define i_keydrop __sp_handlers_destroy
#define i_keyclone __sp_handlers_clone
#include "stc/arc.h"

static inline void *sp_alloc(uint32_t size, void (*value_destroyh)(void *)) {
  /*
    We are creating our custom smart pointer frame which looks sth like this:
     |                   |                      |       |
     | STC smart poniter | struct __sp_handlers | value |
     |                   |                      |       |

    Now we can use ptr to `value` part and go back to smart pointer part
    whenever needed.
  */
  struct my_generic_sp *sp = malloc(sizeof(struct my_generic_sp) +
                                    sizeof(struct __sp_handlers) + size);
  if (!sp) {
    return NULL;
  }

  struct __sp_handlers *sp_handlers = sp + sizeof(struct my_generic_sp);

  void *sp_value =
      sp + sizeof(struct my_generic_sp) + sizeof(struct __sp_handlers);

  *sp_handlers = (struct __sp_handlers){.value = sp_value,
                                        .value_destroyh = value_destroyh};

  *sp = my_generic_sp_from_ptr(sp_handlers);

  return sp_value;
};

static inline struct my_generic_sp *sp_get(void *value) {
  return value - sizeof(struct my_generic_sp) - sizeof(struct __sp_handlers);
};

static inline struct my_generic_sp *
sp_get_from_handlers(struct __sp_handlers *handlers) {
  return handlers - sizeof(struct my_generic_sp);
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
