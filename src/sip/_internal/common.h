/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_SIP_INT_COMMON_H
#define C_MINILIB_SIP_SIP_INT_COMMON_H
#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip/_internal/sip_transaction_list.h"
#include "socket/socket.h"
#include <stdbool.h>

struct cmsu_SipStack {
  socket_t socket; // We need socket in case Sip Stack need to schedule some io.

  list_cmsu_SipTransactions
      transactions; // Transactions are responsible for matching request and
                    // responses into pairs.

  id_gen_t id_gen;
};

bool cmsu_is_sipmsg_request(struct cmsc_SipMessage *sipmsg) {
  return sipmsg->request_line.sip_method.len > 0;
}

bool cmsu_is_sipmsg_response(struct cmsc_SipMessage *sipmsg) {
  return sipmsg->status_line.sip_proto_ver.len > 0;
}

bool cmsu_is_sipmsg_valid(struct cmsc_SipMessage *sipmsg) {
  return cmsu_is_sipmsg_request(sipmsg) || cmsu_is_sipmsg_response(sipmsg);
}

#endif // C_MINILIB_SIP_SIP_COMMON_H
