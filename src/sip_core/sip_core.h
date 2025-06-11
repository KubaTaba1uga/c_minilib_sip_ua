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

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip_transp/sip_transp.h"
#include "utils/ip.h"
#include <stdint.h>

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
typedef struct cmsu_SipCore *sip_core_t;

typedef cme_error_t (*sip_core_request_handler_t)(sip_msg_t sip_msg,
                                                  ip_t peer_ip,
                                                  sip_core_t sip_core,
                                                  void *data);
typedef cme_error_t (*sip_core_response_handler_t)(sip_msg_t sip_msg,
                                                   ip_t peer_ip,
                                                   sip_core_t sip_core,
                                                   void *data);

cme_error_t sip_core_create(event_loop_t evl, ip_t ip_addr,
                            enum SupportedSipTranspProtos sstp,
                            sip_core_t *out);

void sip_core_destroy(sip_core_t *out);

cme_error_t sip_core_listen(sip_core_request_handler_t requesth,
                            sip_core_response_handler_t resph, void *data,
                            sip_core_t sip_core);

#endif // C_MINILIB_SIP_UA_SIP_TRANSP_H
