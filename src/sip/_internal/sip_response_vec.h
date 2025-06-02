/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_RESPONSE_VEC_H
#define C_MINILIB_SIP_UA_SIP_RESPONSE_VEC_H
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "sip/_internal/sip_response.h"

#define i_tag cmsu_SipResponses
#define i_key struct cmsu_SipResponse
#define i_cmp cmsu_SipResponse_cmp
#include <stc/vec.h>

#endif // C_MINILIB_SIP_UA_SIP_RESPONSE_VEC_H
