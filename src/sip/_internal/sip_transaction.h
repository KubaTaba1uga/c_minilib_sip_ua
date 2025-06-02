/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

/*
 * According RFC 3261 17 Transactions:
 *   SIP is a transactional protocol: interactions between components take
 *   place in a series of independent message exchanges. Specifically, a
 *   SIP transaction consists of a single request and any responses to
 *   that request, which include zero or more provisional responses and
 *   one or more final responses.
 */
#ifndef C_MINILIB_SIP_UA_SIP_TRANSACTION_H
#define C_MINILIB_SIP_UA_SIP_TRANSACTION_H

#include "c_minilib_error.h"
#include "sip/_internal/sip_request.h"
#include "sip/_internal/sip_response_vec.h"
#include "sip/sip.h"
#include "utils/id.h"
#include <stdint.h>

/*
 *  Sip transactions are described by few state machines.
 *     - Client Transaction Invite: RFC 3261 17.1.1
 *     - Client Transaction ACK: RFC 3261 17.1
 *     - Client Transaction Other: RFC 3261 17.1.2
 *     - Server Transaction Invite: RFC 3261 17.2.1
 *     - Client Transaction Other: RFC 3261 17.2.2
 */

enum cmsu_SipportedSipTransactions {
  cmsu_SipportedSipTransactions_CLIENT_INVITE,
  cmsu_SipportedSipTransactions_CLIENT_ACK,
  cmsu_SipportedSipTransactions_CLIENT_OTHER,
  cmsu_SipportedSipTransactions_SERVER_INVITE,
  cmsu_SipportedSipTransactions_SERVER_OTHER,
};

struct cmsu_SipTransaction {
  enum cmsu_SipportedSipTransactions type;
  struct cmsu_SipStack *stack;
  id_gen_t id_gen;
  bool is_done;
  void *data;
  id_t id;
};

static inline int cmsu_SipTransaction_cmp(const struct cmsu_SipTransaction *a,
                                          const struct cmsu_SipTransaction *b) {
  if (a->id == b->id) {
    return 0;
  }
  if (a->id > b->id) {
    return 1;
  }

  return -1;
}

#endif // C_MINILIB_SIP_UA_SIP_TRANSACTION_H
