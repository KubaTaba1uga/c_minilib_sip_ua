/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UTILS_STC_WRAPPER_H
#define C_MINILIB_SIP_UA_UTILS_STC_WRAPPER_H
#include "stc/common.h"

// c_foreach does not respect size, leading to non obvious bugs.
// PLS do not use it!!!
#define vec_foreach(it, TYPE, container)                                       \
  c_foreach_n(it, TYPE, (container), (container).size)

#endif // C_MINILIB_SIP_UA_UTILS_STC_WRAPPER_H
