/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_RESPONSES_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_RESPONSES_H
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#include "stc/cstr.h"

struct __SipServerTransactionResponse {
  uint32_t status_code;
  cstr reason_phrase;
};

#define i_tag _SipServerTransactionResponses
#define i_key struct __SipServerTransactionResponse
#include "stc/list.h"

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_RESPONSES_H
