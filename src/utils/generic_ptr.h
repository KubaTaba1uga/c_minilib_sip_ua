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
#include <stdio.h>

// Generic ptrs should never be dropped nor cloned. They does not
// contain customized drop which is required to aviod memory leaks
// on most of structs.
static inline void __GenericPtr_destroy(void **data) { assert(false); };

static inline void *__GenericPtr_clone(void *data) {
  assert(false);
  return data;
};

#define i_type GenericPtr
#define i_key void *
#define i_keydrop __GenericPtr_destroy
#define i_keyclone __GenericPtr_clone
#include "stc/arc.h"

/*
   To cast ARC ptr to generic ptr it has to be created with arc_X_from_ptr func.
    Otherise ptr and data are in one chunk of memory wich leads to issues with
    freeing in STC.
*/
#define GenericPtr_from_arc(TYPE, tptr)                                        \
  ({                                                                           \
    struct GenericPtr out__ = {                                                \
        .use_count = (tptr)->use_count,                                        \
        .get = (void *)((tptr)->get),                                          \
    };                                                                         \
    out__;                                                                     \
  })

#define GenericPtr_dump(TYPE, gptr)                                            \
  ({                                                                           \
    struct TYPE out__ =                                                        \
        (struct TYPE){.use_count = gptr.use_count, .get = (void *)(gptr.get)}; \
    out__;                                                                     \
  })

;
/*
  Generic ptrs functions do not increase or decrease use_count automatically.
    It is done so on purpose, pls do not tinker with this decision. Imagine
    scenarion where you want to pass arg as callback to event loop. Once
    callback is there you want to delete it once you meet error in callback or
    once callback is deleted from external src. Bumping refcount automatically
    makes this simple scenario quite difficult.
*/

#undef GenericPtr_from
#undef GenericPtr_from_ptr
#undef GenericPtr_drop

#endif // C_MINILIB_SIP_UA_UTILS_GENERIC_PTR_H
