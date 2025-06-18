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
#include "utils/ip.h"
#include "utils/sip_msg.h"

/*
  Role of SIP transport is to unifiy data and ops for all transport protocols
  used by SIP.
*/
/******************************************************************************
 *                             Sip Transport                                  *
 ******************************************************************************/
typedef struct __SipTransportPtr *sip_transp_t;
typedef cme_error_t (*sip_transp_recvh_t)(sip_msg_t sip_msg, ip_t peer_ip,
                                          sip_transp_t sip_transp, void *data);
enum SupportedSipTranspProtos {
  SupportedSipTranspProtos_UDP,
};

cme_error_t sip_transp_create(event_loop_t evl, ip_t ip_addr,
                              enum SupportedSipTranspProtos proto_type,
                              sip_transp_t *out);

cme_error_t sip_transp_listen(sip_transp_t sip_transp, sip_transp_recvh_t recvh,
                              void *arg);

#endif // C_MINILIB_SIP_UA_SIP_TRANSP_H
