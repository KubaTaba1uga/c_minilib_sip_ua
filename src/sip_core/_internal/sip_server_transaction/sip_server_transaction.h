/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_SIP_SERVER_TRANSACTION_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_SIP_SERVER_TRANSACTION_H
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"

#include "utils/ip.h"
#include "utils/sip_msg.h"
#include "utils/sip_status_codes.h"

#include "sip_core/_internal/sip_core.h"
#include "timer_fd/timer_fd.h"

#include "sip_core/_internal/sip_server_transaction/sip_server_transaction_responses.h"

enum __SipServerTransactionType {
  __SipServerTransactionType_INVITE,
  __SipServerTransactionType_NONINVITE,
};

enum __SipServerTransactionState {
  __SipServerTransactionState_NONE, // Initial state before anything is sent

  // Invite Server Transaction states
  __SipServerTransactionState_INVITE_PROCEEDING, // After sending 100 Trying or
                                                 // 1xx provisional
  __SipServerTransactionState_INVITE_COMPLETED, // After sending 3xx–6xx final
                                                // response
  __SipServerTransactionState_INVITE_CONFIRMED, // After receiving ACK for
                                                // 3xx–6xx response
  __SipServerTransactionState_INVITE_TERMINATED, // After sending 2xx final
                                                 // response or completing ACK
                                                 // processing

};

struct __SipServerTransaction {
  // Generic ops and data
  enum __SipServerTransactionType type;
  enum __SipServerTransactionState state;
  struct list__SipServerTransactionResponses sip_responses;
  struct SipMessagePtr init_request;
  struct IpAddrPtr last_peer_ip;
  struct SipCorePtr sip_core;

  // We need strans_errh to inform user about canceled transaction
  sip_core_strans_errh_t strans_errh;
  struct GenericPtr strans_errh_arg;

  // Invite Server Transaction ops and data
  struct TimerFdPtr invite_g_timer; // unreliable transport timer
  struct TimerFdPtr invite_h_timer; // ack timer
  struct TimerFdPtr invite_i_timer; // ??
};

void __SipServerTransaction_destroy(struct __SipServerTransaction *sip_strans);
static inline struct __SipServerTransaction
__SipServerTransaction_clone(struct __SipServerTransaction sip_strans) {
  return sip_strans;
};

#define i_type SipServerTransactionPtr
#define i_key struct __SipServerTransaction
#define i_keydrop __SipServerTransaction_destroy
#define i_keyclone __SipServerTransaction_clone
#include "stc/arc.h"

/*
The SIP server transaction is responsible for handling one request
(INVITE/REGISTER/MESSAGE etc.) and managing the reliable delivery of its
responses. Here is overview of reciving operation:

|           ^
| bytes     | bytes
V           |
Socket UDP  <---
|              |
| bytes        | bytes
V              |
Sip Transport  <----
|                  |
| sip message      | bytes
V                  |
Sip Core  <--------------
|                       |
| sip message           | sip message
V                       |
Sip Server Transaction  <-------
|                              |
| sip server transaction       | status code, reason phrase
V                              |
TU------------------------------


Sip server transaction contain a state machine which affect two Server
Transaction behaviours:
  - reciving sip request
  - sending sip response
It is done like so because receiving and sending are both required to complete
server transaction. Server needs to receive request and respond to it after all.
Reciving are arrays pointed downwards on the picture, while sending are arrays
pointed upwords or left.

Server transaction is always created on incoming request with NONE state. Then
sip transaction automatically moves to appropriate state dependently of
transaction type. Then TU reply the transaction. If transaction is accepted TU
passes ops which specify how interact with new requests in transaction ctx.
*/
cme_error_t __SipServerTransactionPtr_create(
    struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct IpAddrPtr last_peer_ip, struct SipServerTransactionPtr *out);

cme_error_t
__SipServerTransactionPtr_recvh(struct SipMessagePtr sip_msg,
                                struct SipServerTransactionPtr *strans);

cme_error_t
__SipServerTransactionPtr_reply(uint32_t status_code, cstr status_phrase,
                                struct SipServerTransactionPtr *strans);

cme_error_t
__SipServerTransactionPtr_get_id(struct SipServerTransactionPtr strans,
                                 struct csview *out);

bool __SipServerTransactionPtr_is_responses_empty(
    struct SipServerTransactionPtr strans);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_SIP_SERVER_TRANSACTION_H
