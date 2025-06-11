/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_STRANS_H
#define C_MINILIB_SIP_UA_SIP_STRANS_H
/*
  This is interface to sip_strans module, if you need anything from sip_strans
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip_core/sip_core.h"
#include "sip_transp/sip_transp.h"
#include "utils/ip.h"
#include <stdint.h>

/******************************************************************************
 *                        Sip Server Transaction                              *
 ******************************************************************************/
typedef struct cmsu_SipServerTransaction *sip_strans_t;

typedef cme_error_t (*sip_strans_request_handler_t)(sip_msg_t sip_msg,
                                                    ip_t peer_ip,
                                                    sip_strans_t sip_strans,
                                                    void *data);
typedef cme_error_t (*sip_strans_response_handler_t)(sip_msg_t sip_msg,
                                                     ip_t peer_ip,
                                                     sip_strans_t sip_strans,
                                                     void *data);

cme_error_t sip_strans_non_invite_create(sip_core_t sip_core,
                                         sip_strans_t *out);

void sip_strans_destroy(sip_strans_t *out);

#endif // C_MINILIB_SIP_UA_SIP_TRANSP_H
