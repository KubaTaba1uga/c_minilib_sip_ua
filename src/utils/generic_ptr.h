/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UTILS_GENERIC_PTR_H
#define C_MINILIB_SIP_UA_UTILS_GENERIC_PTR_H
/*
Each module create it's personalized types as pointers.
 Like Event loop is represented by EventLoopPtr. But sometimes
 we need to pass this personalized ptrs to sam callback or handler
 wich does not know the type under the pointer. That's why we need
 generic ptrs.
*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

// Generic ptrs should never be dropped nor cloned. They does not
// contain customized drop which is required to aviod memory leaks
// on most of structs.
static inline void __GenericPtr_destroy(void **udp_socket) { assert(false); };

static inline void *__GenericPtr_clone(void *udp_socket) {
  assert(false);
  return udp_socket;
};

#define i_type GenericPtr
#define i_key void *
#define i_keydrop __GenericPtr_destroy
#define i_keyclone __GenericPtr_clone
#include "stc/arc.h"

static inline struct GenericPtr __GenericPtr_create(uint32_t usage_count,
                                                    void *data) {
  struct GenericPtr out = GenericPtr_from_ptr(data);
  *out.use_count = usage_count;
  return out;
}

#undef GenericPtr_from

#define GenericPtr_from(TYPE, tptr)                                            \
  ({                                                                           \
    TYPE##_clone(*tptr);                                                       \
    __GenericPtr_create(TYPE##_use_count(tptr), (tptr)->get);                  \
  })

/*  ─── GenericPtr → TYPEPtr ───
    gptr : l-value of struct GenericPtr
    TYPE : target smart-pointer type, e.g. EventLoopPtr          */
#define GenericPtr_dump(TYPE, gptr)                                            \
  ({                                                                           \
    struct TYPE out__ = {.use_count = (gptr).use_count, .get = *(gptr.get)};   \
                                                                               \
    /* poison source so its future drop is a no-op */                          \
    (gptr).get = NULL;                                                         \
    (gptr).use_count = NULL;                                                   \
    out__;                                                                     \
  })

#endif // C_MINILIB_SIP_UA_UTILS_GENERIC_PTR_H
