/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_BUFFER_H
#define C_MINILIB_SIP_UA_BUFFER_H
#include "stc/common.h"
#include "stc/cstr.h"
#include "stc/csview.h"

static inline void __buffer_destroy(cstr *buf_raw) { cstr_drop(buf_raw); }

#define i_type buffer
#define i_key cstr
#define i_keydrop __buffer_destroy
#include "stc/arc.h"

typedef struct buffer buffer_t;

#endif // C_MINILIB_SIP_UA_BUFFER_H
