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
typedef struct cmsu_Ua *ua_t;

cme_error_t ua_create(cmsu_evl_t evl, ua_t *out);
void ua_destroy(ua_t *out);

#endif // C_MINILIB_SIP_UA_SIP_H
