/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#include <stdint.h>
#include <stdio.h>

#include "c_minilib_error.h"

#include "sip_transport/sip_transport.h"
#include "timer_fd/timer_fd.h"
#include "utils/ip.h"
#include "utils/sip_msg.h"

/******************************************************************************
 *                               Sip Listener                                 *
 ******************************************************************************/
struct SipCorePtr;
typedef cme_error_t (*sip_core_request_handler_t)(sip_msg_t sip_msg,
                                                  ip_t peer_ip,
                                                  struct SipTransportPtr strans,
                                                  struct SipCorePtr sip_core,
                                                  void *data);

struct __SipCoreListener {
  sip_core_request_handler_t request_handler;
  void *arg;
};

#define i_tag _SipCoreListenersQueue
#define i_key struct __SipCoreListener
#include "stc/queue.h"

/******************************************************************************
 *                               Sip Server Transaction                       *
 ******************************************************************************/
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
  struct TimerFdPtr invite_100_timer;
  struct TimerFdPtr invite_3xx_6xx_timer;
  struct TimerFdPtr invite_retransmission_timer;
  struct __SipCore *sip_core; // We need only weak reference to SipCore
                              // Because Transaction memory is owned by
                              // __SipCore->stranses wich guarantee that
                              // Transaction cannot live without core.
                              // Once core is destroyed same the transaction.
  sip_msg_t request;
  bool is_invite;
  ip_t last_peer_ip;
};

void __SipServerTransaction_destroy(void *data);
struct __SipServerTransaction
__SipServerTransaction_clone(struct __SipServerTransaction sip_strans);

#define i_type SipServerTransactionPtr
#define i_key struct __SipServerTransaction
#define i_keydrop __SipServerTransaction_destroy
#define i_keyclone __SipServerTransaction_clone
#include "stc/arc.h"

#define i_tag _SipServerTransactionsHMap
#define i_keypro cstr
#define i_val struct SipServerTransactionPtr
#define i_valdrop SipServerTransactionPtr_drop
#define i_valclone SipServerTransactionPtr_clone
#include "stc/hmap.h"

/******************************************************************************
 *                               Sip Core                                     *
 ******************************************************************************/
struct __SipCore {
  // Transp Protocol data
  struct EventLoopPtr evl;
  struct SipTransportPtr sip_transp;

  // SIP core data
  struct queue__SipCoreListenersQueue *listeners;
  struct hmap__SipServerTransactionsHMap *stranses;
};

void __SipCore_destroy(void *data);
struct __SipCore __SipCore_clone(struct __SipCore sip_core);

#define i_type SipCorePtr
#define i_key struct __SipCore
#define i_keydrop __SipCore_destroy
#define i_keyclone __SipCore_clone
#include "stc/arc.h"

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
