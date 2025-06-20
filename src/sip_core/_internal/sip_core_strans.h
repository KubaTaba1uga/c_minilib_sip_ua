/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"

#include "utils/ip.h"
#include "utils/sip_msg.h"

#include "sip_core/_internal/sip_core.h"

cme_error_t SipServerTransaction_create(sip_msg_t sip_msg,
                                        struct SipCorePtr sip_core,
                                        ip_t last_peer_ip,
                                        struct SipServerTransactionPtr *out);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
