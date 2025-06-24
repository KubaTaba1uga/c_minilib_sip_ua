/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_CORE_H
#define C_MINILIB_SIP_UA_SIP_CORE_H
/*
  This is interface to sip_core module, if you need anything from sip_core
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */
#include <stdint.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip_transport/sip_transport.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"

#include "sip_core/_internal/sip_core.h"

/******************************************************************************
 *                               Sip Core                                     *
 ******************************************************************************/
struct SipServerTransactionPtr;
typedef cme_error_t (*sip_core_connh_t)(
    struct SipMessagePtr sip_msg, struct IpAddrPtr peer_ip,
    struct SipCorePtr *sip_core, struct SipServerTransactionPtr *sip_strans,
    struct GenericPtr data);

typedef cme_error_t (*sip_core_reqh_t)(
    struct SipMessagePtr sip_msg, struct SipCorePtr *sip_core,
    struct SipServerTransactionPtr *sip_strans, struct GenericPtr arg);

cme_error_t SipCorePtr_create(struct EventLoopPtr evl, struct IpAddrPtr ip_addr,
                              enum SipTransportProtocolType proto_type,
                              struct SipCorePtr *out);
/*
 SipCore_listen set up recvh on sip stack created in SipCore_create. So every
 time SIP receives valid msg starting new SIP transaction it will fire up
 SipCore recvh which as consequence fire up `sip_core_connh_t
 requesth` with sevrer transaction next to sip msg from SipTransport layer.
 Chain looks sth like:

     EventLoop
        |
        | POLLIN signal
        |
        V
     UdpSocket
        |
        | bytes
        |
        V
     SipTransport
        |
        | SipMessage
        |
        V
     SipCore
        |
        | SipMsg, SipServerTransaction
        |
        V
     `sip_core_connh_t requesth`

 TU interacts directly with Sip Core. Main goal of Sip Core is to allow
 for implementing different TU operations quickly and easilly. Sip Core
 handles matching requests to responses via transactions.
*/
cme_error_t SipCorePtr_listen(sip_core_connh_t connh, struct GenericPtr arg,
                              struct SipCorePtr sip_core);

/* cme_error_t sip_send(sip_core_response_handler_t resph, struct SipMessagePtr
 * sipmsg, */
/*                      void *data, struct SipCorePtr sip_core); */

#endif // C_MINILIB_SIP_UA_SIP_CORE_H
