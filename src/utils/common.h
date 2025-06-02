/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_COMMON_H
#define C_MINILIB_SIP_UA_COMMON_H
#include <inttypes.h>

#define CMSU_SOCKETS_MAX 1024

typedef struct {
  char *buf;
  uint32_t len;
} buffer_t;
typedef struct cmsu_EventLoop *evl_t;

#endif // C_MINILIB_SIP_UA_COMMON_H
