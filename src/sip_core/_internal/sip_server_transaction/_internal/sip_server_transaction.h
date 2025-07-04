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

#include "sip_core/_internal/sip_server_transaction/_internal/sip_server_transaction_responses.h"

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
  struct SipCorePtr sip_core;
  enum __SipServerTransactionType __type;
  enum __SipServerTransactionState __state;
  struct list__SipServerTransactionResponses __sip_responses;
  struct SipMessagePtr __init_request;
  struct IpAddrPtr __last_peer_ip;

  // We need errh to inform user about errors in transaction
  sip_core_strans_errh_t __errh;
  struct GenericPtr __errh_arg;

  // Invite Server Transaction ops and data
  struct TimerFdPtr __invite_g_timer; // unreliable transport timer
  struct TimerFdPtr __invite_h_timer; // ack timer
  struct TimerFdPtr __invite_i_timer; // ??
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

|                                ^
| bytes                          | bytes
V                                |
Socket UDP                       Socket Udp
|                                ^
| bytes                          | bytes
V                                |
Sip Transport                    Sip Transport
|                                ^
| sip message                    | bytes
V                                |
Sip Core                         Sip Core
|                                ^
| sip message, sip core          | sip msg, sip core
V                                |
Sip Listener                     Sip Server Transaction
|                                ^
| sip message, sip core,         | status code, reason phrase,
| sip server transaction         | sip server transaction
V                                |
TU -------------------------------


Sip server transaction contain a state machine which affect two Server
Transaction behaviours:
  - reciving sip request
  - sending sip response
It is done like so because receiving and sending are both required to complete
server transaction. Server needs to receive request and respond to it after all.
Reciving are arrays pointed downwards on the picture, while sending are arrays
pointed upwords.

Server transaction is always created on incoming request with NONE state. Then
sip transaction automatically moves to appropriate state dependently of
transaction type. Then TU reply the transaction. If transaction is accepted
it get terminated immidiatly making TU responsible for retransmissions and ACK
handling. If transaction is rejected (3xx-6xx, especially 4xx) transaction
handles retransmissions and ACK handling.
*/
cme_error_t __SipServerTransactionPtr_create(
    struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct IpAddrPtr peer_ip, struct SipServerTransactionPtr *out);

cme_error_t
__SipServerTransactionPtr_recvh(struct SipMessagePtr sip_msg,
                                struct IpAddrPtr peer_ip,
                                struct SipServerTransactionPtr *strans);

cme_error_t
__SipServerTransactionPtr_get_id(struct SipServerTransactionPtr strans,
                                 struct csview *out);

bool __SipServerTransactionPtr_is_responses_empty(
    struct SipServerTransactionPtr strans);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_SIP_SERVER_TRANSACTION_H
