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
#include "subprojects/c_minilib_sip_codec/include/c_minilib_sip_codec.h"

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

static inline void cmsu_SipCtx_destroy(void *ctx) {
  if (!ctx) {
    return;
  }

  free(ctx);
};

static inline cme_error_t cmsu_sip_recvh(uint32_t buf_len, char *buf,
                                         void *ctx) {
  struct cmsc_SipMessage *sipmsg;
  cme_error_t err;

  err = cmsc_parse_sip(buf_len, buf, &sipmsg);
  if (err) {
    goto error_out;
  }

  puts("HIT receive");
  return 0;

error_out:
  return cme_return(err);
};
static inline cme_error_t cmsu_sip_sendh(uint32_t buf_len, char *buf,
                                         void *ctx) {
  puts("HIT send");
  return 0;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_H
