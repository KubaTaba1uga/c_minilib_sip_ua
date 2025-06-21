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
typedef cme_error_t (*sip_core_request_handler_t)(
    sip_msg_t sip_msg, ip_t peer_ip, struct SipCorePtr *sip_core,
    struct SipServerTransactionPtr *sip_strans, struct GenericPtr data);

cme_error_t SipCorePtr_create(struct EventLoopPtr evl, ip_t ip_addr,
                              enum SipTransportProtocolType proto_type,
                              struct SipCorePtr *out);

cme_error_t SipCorePtr_listen(sip_core_request_handler_t requesth,
                              struct GenericPtr data,
                              struct SipCorePtr sip_core);

/* cme_error_t sip_send(sip_core_response_handler_t resph, sip_msg_t sipmsg, */
/*                      void *data, struct SipCorePtr sip_core); */

#endif // C_MINILIB_SIP_UA_SIP_CORE_H
