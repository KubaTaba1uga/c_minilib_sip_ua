/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_REQUEST_LIST_H
#define C_MINILIB_SIP_UA_SIP_REQUEST_LIST_H
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "sip/_internal/sip_request.h"

typedef struct cmsu_SipRequest cmsu_SipRequest;
#define i_tag cmsu_SipRequests
#define i_key cmsu_SipRequest
#define i_cmp cmsu_SipRequest_cmp
#include <stc/vec.h>

#endif // C_MINILIB_SIP_UA_SIP_REQUEST_LIST_H
