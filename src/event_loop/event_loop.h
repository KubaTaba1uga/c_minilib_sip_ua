/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_H
/*
  This is interface to event_loop module, if you need anything from event_loop
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */

#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "socket/socket.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/

typedef struct cmsu_EventLoop *event_loop_t;

cme_error_t event_loop_create(event_loop_t *out);
cme_error_t event_loop_start(event_loop_t evl);
void event_loop_destroy(event_loop_t *out);

cme_error_t event_loop_insert_socket(socket_t socket, short events,
                                     event_loop_t evl);
void event_loop_remove_socket(socket_t socket);
void event_loop_async_send_socket(socket_t socket);

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
