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
  enum __SipServerTransactionType type;
  enum __SipServerTransactionState state;
  struct TimerFdPtr invite_retransmission_timer;
  struct TimerFdPtr invite_3xx_6xx_timer;
  struct TimerFdPtr invite_trying_timer;
  struct IpAddrPtr last_peer_ip;
  struct SipMessagePtr last_response;
  struct SipMessagePtr init_request;

  struct SipCorePtr sip_core;
};

void __SipServerTransaction_destroy(void *data);
static inline struct __SipServerTransaction
__SipServerTransaction_clone(struct __SipServerTransaction sip_strans) {
  return sip_strans;
};

#define i_type SipServerTransactionPtr
#define i_key struct __SipServerTransaction
#define i_keydrop __SipServerTransaction_destroy
#define i_keyclone __SipServerTransaction_clone
#include "stc/arc.h"

cme_error_t SipServerTransactionPtr_create(struct SipMessagePtr sip_msg,
                                           struct SipCorePtr sip_core,
                                           struct IpAddrPtr last_peer_ip,
                                           struct SipServerTransactionPtr *out);

cme_error_t
SipServerTransactionPtr_internal_reply(uint32_t status_code, cstr status,
                                       // TO-DO add more sip-msg args to fill
                                       struct SipServerTransactionPtr strans);

cme_error_t
SipServerTransactionPtr_tu_reply(uint32_t status_code, cstr status,
                                 // TO-DO add more sip-msg args to fill
                                 struct SipServerTransactionPtr strans);

cme_error_t
SipServerTransactionPtr_next_state(struct SipMessagePtr sip_msg,
                                   struct SipServerTransactionPtr strans);

static inline cme_error_t
SipServerTransactionPtr_get_branch(struct SipServerTransactionPtr strans,
                                   struct csview *out) {
  cme_error_t err;

  void *result = SipMessagePtr_get_branch(strans.get->init_request, out);
  if (!result) {
    err =
        cme_error(ENODATA, "Missing via->branch in server transaction request");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
