/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SOCKET_H
#define C_MINILIB_SIP_UA_SOCKET_H
#include <inttypes.h>
#include <stdint.h>

#include "utils/ip.h"

typedef cme_error_t (*socket_recvh_t)(char *buffer, int32_t buffer_len, ip_t peer,
                                   void *socket, void *data);

#endif // C_MINILIB_SIP_UA_SOCKET_H
