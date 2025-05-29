/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_H
#define C_MINILIB_SIP_UA_INT_SIP_H

#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "socket/socket.h"

struct cmsu_SipCtx {
  cmsu_sock_t socket;
};

static inline cme_error_t cmsu_SipCtx_create(struct cmsu_SipCtx **ctx) {
  struct cmsu_SipCtx *tmp = calloc(1, sizeof(struct cmsu_SipCtx));
  if (!tmp) {
    return cme_return(
        cme_error(ENOMEM, "Cannot allocate memory for `sip_ctx`"));
  }

  *ctx = tmp;

  return 0;
};

static inline cme_error_t cmsu_sip_recv_calbck(uint32_t buf_len, char *buf,
                                               void *ctx) {
  puts("HIT receive");
  return 0;
};
static inline cme_error_t cmsu_sip_send_calbck(uint32_t buf_len, char *buf,
                                               void *ctx) {
  puts("HIT send");
  return 0;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_H
