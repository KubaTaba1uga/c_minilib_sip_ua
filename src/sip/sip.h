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
#include "socket/socket.h"
#include "utils/common.h"

/******************************************************************************
 *                             Sip                                            *
 ******************************************************************************/
typedef struct cmsu_SipCtx *sip_t;

typedef struct {
  // Data
  cmsu_evl_t evl;

  // Ops
  cme_error_t (*recv_callback)(cmsu_evl_t evl, ip_addr_t sender,
                               struct cmsc_SipMessage *sipmsg, sip_t sip);
  cme_error_t (*send_callback)(cmsu_evl_t evl, ip_addr_t recver,
                               struct cmsc_SipMessage *sipmsg, sip_t sip);
  void(*ctx_destroy);
} sip_create_arg;
cme_error_t sip_create(sip_create_arg args, socket_t *out);

#endif // C_MINILIB_SIP_UA_SIP_H
