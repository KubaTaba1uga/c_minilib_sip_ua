/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#include "sip_core/_internal/common.h"

cme_error_t __SipCore_listen(sip_core_request_handler_t reqh, void *data,
                             struct SipCorePtr sip_core);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
