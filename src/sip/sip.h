/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_H
#define C_MINILIB_SIP_UA_SIP_H
/*
  This is interface to sip module, if you need anything from sip module,
   put interface declaration here and interface implementation in .c but always
   write real implementation as static inline in _internal.
 */

#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "socket/socket.h"
#include "utils/common.h"

/******************************************************************************
 *                             Sip                                            *
 ******************************************************************************/
typedef struct cmsu_SipSession *sip_session_t;

// TO-DO add recv and send callbacks
cme_error_t sip_create_session(cmsu_evl_t evl, ip_addr_t ipaddr,
                               sip_session_t *out);
void sip_destroy_session(void *sipsess);

#endif // C_MINILIB_SIP_UA_SIP_H
