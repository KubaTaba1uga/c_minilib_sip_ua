/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_REQUEST_QUEUE_H
#define C_MINILIB_SIP_UA_REQUEST_QUEUE_H
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "socket/_internal/request.h"

typedef struct cmsu_Request cmsu_Request;
#define i_tag cmsu_Requests
#define i_key cmsu_Request
#define i_cmp cmsu_Request_cmp
#include <stc/queue.h>

#endif
