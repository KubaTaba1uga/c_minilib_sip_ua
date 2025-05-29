/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_H
/*
  This is interface to event loop module, if you need anything from event loop
   module, put here and in .c interface and write implementation as static
   inline in _internal.
 */

#include "c_minilib_error.h"
#include "socket/socket.h"
#include "utils/error.h"
#include <stdint.h>

cme_error_t cmsu_event_loop_start(void);

cme_error_t cmsu_event_loop_insert_udp_socket(
    const char *ipaddr, uint32_t port, void *ctx,
    cme_error_t (*recv_calbck)(uint32_t buf_len, char *buf, void *ctx),
    cme_error_t (*send_calbck)(uint32_t buf_len, char *buf, void *ctx),
    cmsu_sock_t *out);

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
