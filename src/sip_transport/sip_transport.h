/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_TRANSP_H
#define C_MINILIB_SIP_UA_SIP_TRANSP_H
/*
  This is interface to sip_transp module, if you need anything from sip_transp
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */
#include <stdint.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#include "event_loop/event_loop.h"
#include "utils/buffer_ptr.h"
#include "utils/ip.h"
#include "utils/sip_msg.h"

#include "sip_transport/_internal/sip_transport.h"

/*
  Role of SIP transport is to unifiy data and ops for all transport protocols
  used by SIP.
*/
/******************************************************************************
 *                             Sip Transport                                  *
 ******************************************************************************/
#define i_type SipTransportPtr
#define i_key struct __SipTransport
#define i_keydrop __SipTransport_destroy
#define i_keyclone __SipTransport_clone
#include "stc/arc.h"

cme_error_t SipTransportPtr_create(struct EventLoopPtr evl,
                                   struct IpAddrPtr ip_addr,
                                   enum SipTransportProtocolType proto_type,
                                   struct SipTransportPtr *out);
/*
 SipTransport_listen set up recvh on appropriate socket pointed by
 SipTransport->proto_type. So every time socket receives some data from Linux
 Kernel it will fire up SipTransport recvh which as consequence fire up
 `sip_transp_recvh_t recvh` with sip_msg instead of bytes. Chain looks sth
 like:

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
     `sip_transp_recvh_t recvh`

*/
cme_error_t SipTransportPtr_listen(struct SipTransportPtr sip_transpp,
                                   sip_transp_recvh_t recvh,
                                   struct GenericPtr arg);

cme_error_t SipTransportPtr_send(struct SipTransportPtr sip_transpp,
                                 struct IpAddrPtr ip_addr,
                                 struct BufferPtr bytes);

bool SipTransportPtr_is_reliable(struct SipTransportPtr sip_transpp);

#endif // C_MINILIB_SIP_UA_SIP_TRANSP_H
