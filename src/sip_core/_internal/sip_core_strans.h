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
#include "timer_fd/timer_fd.h"

enum __SipServerTransactionState {
  __SipServerTransactionState_TRYING, // Initial state before anything is sent
  __SipServerTransactionState_PROCEEDING, // After sending 100 Trying or 1xx
                                          // provisional
  __SipServerTransactionState_COMPLETED, // After sending 3xx–6xx final response
  __SipServerTransactionState_CONFIRMED,  // After receiving ACK for 3xx–6xx
                                          // response
  __SipServerTransactionState_TERMINATED, // After sending 2xx final response or
                                          // completing ACK processing

};

struct __SipServerTransaction {
  enum __SipServerTransactionState state;
  struct TimerFdPtr invite_retransmission_timer;
  struct TimerFdPtr invite_3xx_6xx_timer;
  struct TimerFdPtr invite_100_timer;
  struct SipCorePtr sip_core;
  ip_t last_peer_ip;
  sip_msg_t request;
  bool is_invite;
};

void __SipServerTransaction_destroy(void *data);
struct __SipServerTransaction
__SipServerTransaction_clone(struct __SipServerTransaction sip_strans);

#define i_type SipServerTransactionPtr
#define i_key struct __SipServerTransaction
#define i_keydrop __SipServerTransaction_destroy
#define i_keyclone __SipServerTransaction_clone
#include "stc/arc.h"

cme_error_t
SipServerTransactionPtr_create(sip_msg_t sip_msg, struct SipCorePtr *sip_core,
                               ip_t last_peer_ip,
                               struct SipServerTransactionPtr **out);

cme_error_t
SipServerTransactionPtr_next_state(sip_msg_t sip_msg,
                                   struct SipServerTransactionPtr *strans);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
