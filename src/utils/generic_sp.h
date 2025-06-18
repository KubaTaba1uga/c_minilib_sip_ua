/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SMART_PTR_H
#define C_MINILIB_SIP_UA_SMART_PTR_H

#include <stddef.h>
#include <stdlib.h>

#include "stc/common.h"

#define container_of(ptr, type, member)                                        \
  ((type *)((char *)(ptr)-offsetof(type, member)))

typedef void *(*__GenericSp_alloch_t)(uint32_t);
typedef void (*__GenericSp_destroyh_t)(void *);
struct __GenericSp {
  void *stc_arc;
  void *data;
  void (*data_destroyh)(void *);
};

static inline void __GenericSp_destroy(struct __GenericSp *sp) {
  if (!sp) {
    return;
  }

  if (sp->data && sp->data_destroyh) {
    sp->data_destroyh(sp->data);
  }

  sp->data = NULL;
  sp->stc_arc = NULL;
  sp->data_destroyh = NULL;
};

static inline struct __GenericSp *__GenericSp_clone(struct __GenericSp *sp) {
  return sp;
}

#define i_type my_generic_sp
#define i_key struct __GenericSp
#define i_keydrop __GenericSp_destroy
#define i_keyclone __GenericSp_clone
#include "stc/arc.h"

static inline void **__GenericSp_alloc(uint32_t size,
                                       __GenericSp_destroyh_t value_destroyh,
                                       __GenericSp_alloch_t alloc_fn) {
  // Watch out: bad code!!
  // This code uses too much mallocs, maybe some day we should move to ARC from
  // STC. I tried this but this require making all types public, which kindo
  // suck. Opaque pointer is cool pattern which improves code quality i wouldn't
  // like to drop it. But if these mallocs will become bottleneck then probably
  // this is what we should do.
  struct my_generic_sp *sp = alloc_fn(sizeof(struct my_generic_sp));
  if (!sp) {
    goto error_out;
  }

  struct __GenericSp *pl = alloc_fn(sizeof(struct __GenericSp));
  if (!pl) {
    goto error_sp_cleanup;
  }

  void *data = alloc_fn(size);
  if (!data) {
    goto error_pl_cleanup;
  }

  *pl = (struct __GenericSp){
      .data = data,
      .value_destroyh = value_destroyh,
      .sp = sp,
  };

  *sp = my_generic_sp_from_ptr(pl);

  return &pl->data;

error_pl_cleanup:
  free(pl);
error_sp_cleanup:
  free(sp);
error_out:
  return NULL;
}

static inline void *sp_malloc(uint32_t size,
                              __GenericSp_destroyh_t value_destroyh) {
  return __GenericSp_alloc(size, value_destroyh, (__GenericSp_alloch_t)malloc);
}

static void *__GenericSp_calloc_adapter(uint32_t n) { return calloc(1, n); }

static inline void *sp_zalloc(uint32_t size,
                              __GenericSp_destroyh_t value_destroyh) {
  return __GenericSp_alloc(size, value_destroyh, __GenericSp_calloc_adapter);
}

static inline void **sp_from(void *ptr, __GenericSp_destroyh_t value_destroyh) {
  struct my_generic_sp *sp = malloc(sizeof(struct my_generic_sp));
  if (!sp) {
    goto error_out;
  }

  struct __GenericSp *pl = malloc(sizeof(struct __GenericSp));
  if (!pl) {
    goto error_sp_cleanup;
  }

  *pl = (struct __GenericSp){
      .data = ptr,
      .value_destroyh = value_destroyh,
      .sp = sp,
  };

  *sp = my_generic_sp_from_ptr(pl);

  return &pl->data;

error_sp_cleanup:
  free(sp);
error_out:
  return NULL;
};

static inline struct my_generic_sp *sp_dump(void **data_ptr) {
  return container_of(data_ptr, struct __GenericSp, data)->stc_arc;
}

static inline void *sp_ref(void **value) {
  if (!value || !*value) {
    return value;
  }

  struct my_generic_sp *sp = sp_dump(value);

  my_generic_sp_clone(*sp);

  return value;
}

static inline void sp_deref(void *value) {
  if (!value || !*value) {
    return;
  }

  struct my_generic_sp *sp = sp_dump(value);
  uint32_t use_count = my_generic_sp_use_count(sp);

  my_generic_sp_drop(sp);

  if (--use_count == 0) {
    free(sp);
  }
}

#endif //  C_MINILIB_SIP_UA_SMART_PTR_H
