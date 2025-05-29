/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <stdio.h>

#include "c_minilib_error.h"
#include "c_minilib_init.h"
#include "event_loop/event_loop.h"
#include "sip/_internal/sip.h"
#include "socket/socket.h"

static int cmsu_sip_init(void) {
  struct cmsu_SipCtx *sip_ctx;
  cme_error_t err;

  err = cmsu_SipCtx_create(&sip_ctx);
  if (err) {
    goto error_out;
  }

  err = cmsu_event_loop_insert_udp_socket(
      "127.0.0.1", 7337,
      (struct cmsu_SocketArg){.ctx = sip_ctx,
                              .recvh = cmsu_sip_recvh,
                              .sendh = cmsu_sip_sendh,
                              .destroyh = cmsu_SipCtx_destroy},
      &sip_ctx->socket);

  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return err->code;
}

static void cmsu_sip_destroy(void) {}

CMI_REGISTER(sip, cmsu_sip_init, cmsu_sip_destroy,
             (const char *[]){"event_loop"}, 1);
