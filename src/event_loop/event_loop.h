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

#include <poll.h>
#include <stdint.h>
#include <sys/poll.h>

#include "c_minilib_error.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
typedef struct __EventLoopPtr *event_loop_t;
typedef cme_error_t (*event_loop_sendh_t)(void *data);
typedef cme_error_t (*event_loop_recvh_t)(void *data);
typedef cme_error_t (*event_loop_timeouth_t)(void *data);

cme_error_t event_loop_create(event_loop_t *out);

cme_error_t event_loop_insert_socketfd(event_loop_t evlp, uint32_t fd,
                                       event_loop_sendh_t sendh,
                                       event_loop_recvh_t recvh, void *data);

void event_loop_remove_fd(event_loop_t evlp, int32_t fd);

cme_error_t event_loop_set_pollin(event_loop_t evlp, int32_t fd);

cme_error_t event_loop_set_pollout(event_loop_t evlp, int32_t fd);

cme_error_t event_loop_unset_pollout(event_loop_t evlp, int32_t fd);

cme_error_t event_loop_start(event_loop_t evlp);

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
