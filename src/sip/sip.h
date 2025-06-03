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
#include "utils/id.h"

/******************************************************************************
 *                             Sip                                            *
 ******************************************************************************/
typedef struct cmsu_SipStack *sip_stack_t;
typedef struct cmsu_SipTransaction *sip_trans_t;
typedef struct cmsc_SipMessage *sip_msg_t;

// TO-DO add recv and send callbacks
cme_error_t sip_stack_create(evl_t evl, ip_addr_t ip_addr, sip_stack_t *out);
void sip_stack_destroy(void *sip_stack);
id_t sip_stack_gen_id(sip_stack_t sip_stack);
cme_error_t sip_stack_connect(sip_stack_t sip_stack, ip_addr_t recver,
                              sip_msg_t msg);

#endif // C_MINILIB_SIP_UA_SIP_H
