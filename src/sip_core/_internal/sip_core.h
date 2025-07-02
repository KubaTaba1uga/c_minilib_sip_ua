/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_H
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"
#include "utils/ip.h"

/******************************************************************************
 *                               Sip Core                                     *
 ******************************************************************************/
struct __SipCore {
  // Transp Protocol data
  struct EventLoopPtr evl;
  struct SipTransportPtr sip_transp;

  // SIP core data
  struct queue__SipCoreListeners *listeners;
  struct hmap__SipServerTransactions *stranses;
};

void __SipCore_destroy(struct __SipCore *sip_core);
static inline struct __SipCore __SipCore_clone(struct __SipCore sip_core) {
  return sip_core;
};

#define i_type SipCorePtr
#define i_key struct __SipCore
#define i_keydrop __SipCore_destroy
#define i_keyclone __SipCore_clone
#include "stc/arc.h"

struct SipServerTransactionPtr;
typedef cme_error_t (*sip_core_reqh_t)(struct SipMessagePtr sip_msg,
                                       struct IpAddrPtr peer_ip,
                                       struct SipCorePtr sip_core,
                                       struct GenericPtr arg);

typedef void (*sip_core_strans_errh_t)(
    cme_error_t err, struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_H
