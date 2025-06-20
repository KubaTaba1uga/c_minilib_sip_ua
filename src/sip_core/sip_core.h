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
#include "sip_core/sip_core.h"
#include "sip_transport/sip_transport.h"
#include "utils/ip.h"

/******************************************************************************
 *                               Sip Core                                     *
 ******************************************************************************/

cme_error_t SipCorePtr_create(struct EventLoopPtr evl, ip_t ip_addr,
                              enum SipTransportProtocolType proto_type,
                              struct SipCorePtr *out);

struct SipCorePtr SipCorePtr_ref(struct SipCorePtr sip_corep);

void SipCorePtr_deref(struct SipCorePtr sip_corep);

cme_error_t SipCorePtr_listen(sip_core_request_handler_t requesth, void *data,
                              struct SipCorePtr sip_core);

/* cme_error_t sip_send(sip_core_response_handler_t resph, sip_msg_t sipmsg, */
/*                      void *data, struct SipCorePtr sip_core); */

#endif // C_MINILIB_SIP_UA_SIP_CORE_H
