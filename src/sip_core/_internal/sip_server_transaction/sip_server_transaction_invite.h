/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_INVITE_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_INVITE_H
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"

#include "utils/ip.h"
#include "utils/sip_msg.h"

#include "sip_core/_internal/sip_core.h"
#include "timer_fd/timer_fd.h"

cme_error_t __SipServerTransactionPtr_invite_recv_next_state(
    struct SipServerTransactionPtr strans);

#endif
