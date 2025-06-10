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

cme_error_t sip_core_create(event_loop_t evl, ip_t ip_addr,
                            enum SupportedSipTranspProtos sstp,
                            sip_core_t *out);

// Core listen needs to take request handler wich will be triggered on every
// request. This way one can implement different sip transactions on top of
// sip core.
cme_error_t sip_core_listen(event_loop_t evl, ip_t ip_addr, sip_core_t *out);

#endif // C_MINILIB_SIP_UA_SIP_TRANSP_H
