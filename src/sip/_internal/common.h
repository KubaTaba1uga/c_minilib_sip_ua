/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_SIP_INT_COMMON_H
#define C_MINILIB_SIP_SIP_INT_COMMON_H
#include "c_minilib_error.h"
#include "sip/_internal/sip_transaction_list.h"
#include "socket/socket.h"

struct cmsu_SipStack {
  socket_t socket; // We need socket in case Sip Stack need to schedule some io.

  list_cmsu_SipTransactions
      transactions; // Transactions are responsible for matching request and
                    // responses into pairs.

  id_gen_t id_gen;
};

#endif // C_MINILIB_SIP_SIP_COMMON_H
