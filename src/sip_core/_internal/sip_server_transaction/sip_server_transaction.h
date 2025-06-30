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
#include "utils/sip_status_codes.h"

#include "sip_core/_internal/sip_core.h"
#include "timer_fd/timer_fd.h"

struct SipCoreAcceptOps {
  sip_core_reqh_t reqh;
  sip_core_errh_t errh;
  struct GenericPtr arg;
};

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
  struct SipMessagePtr last_response;
  struct SipMessagePtr init_request;
  struct SipCoreAcceptOps tu_ops;
  struct IpAddrPtr last_peer_ip;
  struct SipCorePtr sip_core;

  // Invite Server Transaction ops and data
  struct TimerFdPtr invite_3xx_6xx_timer;
  struct TimerFdPtr invite_ack_timer;
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
Sip server transaction contain a state machine which affect two Server Transaction behaviours:
  - reciving sip request
  - sending sip response
It is done like so because receiving and sending are both required to complete server transaction.
Server needs to receive request and respond to it after all.

Server transaction is always created on incoming request with NONE state. Then sip transaction
automatically moves to appropriate state dependently of transaction type. Then TU accepts/reject
the transaction. If transaction is accepted TU passes ops which specify how interact with new.
Handling of incoming request looks
sth like this:

Sip Transport
|
| __SipCore_sip_transp_recvh(...)
Sip Core
|
| __SipServerTransactionPtr_recvh(...)
V
Sip server transaction
|
|
---------------------------
|                         |
| invite_recvh(...)       | noninvite_recvh(...)
V                         V
Sip server transaction    Sip server transaction
|                         |
|                         |
V                         V
---------------------------
|                          
| return
V
Sip Core
|
|
V
TU
|
|
-----------------------------------------------------------------------------
|                                                                           |
| reply(200)                                                                | reply(486)
V                                                                           V
Sip Core                                                                    Sip core
|                                                                           |
| reply(200)                                                                | reply(486)
V                                                                           V
Sip server transaction                                                      Sip server transaction
|                                                                           |
|                                                                           |
-------------------------------------                                       -------------------------------------
|                                   |                                       |                                   |
| invite_reply(200)                 | noninvite_reply(200)                  | invite_reply(486)                 | noninvite_reply(486)
V                                   V                                       V                                   V
Sip server transaction             Sip server transaction                   Sip server transaction             Sip server transaction
|                                   |                                       |                                   |
| invite_next_state(TERMINATED)     | noninvite_next_state(COMPLETED)       | invite_next_state(CONFIRMED)      | noninvite_next_state(COMPLETED)
V                                   V                                       V                                   V
-------------------------------------                                       -------------------------------------
|                                                                           |
|                                                                           |
-----------------------------------------------------------------------------
|
| return
V                                         
Sip Core
|
| return
V                                         
TU
|
| return 
V
Sip Core
|
|
V
Sip Transport

Notes:
  - We need X_reply so X transaction can detect failure in transport send. It is required
    in moving transaction state to TERMINATED in case of error.
  - 
*/
cme_error_t SipServerTransactionPtr_create(struct SipMessagePtr sip_msg,
                                           struct SipCorePtr sip_core,
                                           struct IpAddrPtr last_peer_ip,
                                           struct SipServerTransactionPtr *out);

cme_error_t
SipServerTransactionPtr_recvh(struct SipMessagePtr sip_msg,
                                     struct SipServerTransactionPtr strans);

cme_error_t
SipServerTransactionPtr_reply(uint32_t status_code, cstr status_phrase,
                              struct SipServerTransactionPtr strans);

cme_error_t
SipServerTransactionPtr_get_id(struct SipServerTransactionPtr strans,
                               struct csview *out);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
