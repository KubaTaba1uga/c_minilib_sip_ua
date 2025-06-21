/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_MAP_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_MAP_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#include "sip_core/_internal/sip_core_strans.h"
#include "utils/sip_msg.h"

#define i_tag _SipServerTransactions
#define i_keypro cstr
#define i_val struct SipServerTransactionPtr
#define i_valdrop SipServerTransactionPtr_drop
#define i_valclone SipServerTransactionPtr_clone
#include "stc/hmap.h"

cme_error_t
SipServerTransactions_insert(csview key, struct SipServerTransactionPtr value,
                             struct hmap__SipServerTransactions *hmap,
                             struct SipServerTransactionPtr **out);

struct SipServerTransactionPtr *
SipServerTransactions_find(csview key,
                           struct hmap__SipServerTransactions *stmap);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_MAP_H
