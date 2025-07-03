/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_H
#include <poll.h>
#include <stdint.h>
#include <sys/poll.h>

#include "c_minilib_error.h"

#include "event_loop/_internal/event_loop.h"
#include "utils/generic_ptr.h"
/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
cme_error_t EventLoopPtr_create(struct EventLoopPtr *out);

cme_error_t EventLoopPtr_insert_socketfd(struct EventLoopPtr evlp, uint32_t fd,
                                         event_loop_recvh_t recvh,
                                         struct GenericPtr data);

cme_error_t EventLoopPtr_insert_timerfd(struct EventLoopPtr evlp, uint32_t fd,
                                        event_loop_timeouth_t timeouth,
                                        struct GenericPtr data);

void EventLoopPtr_remove_fd(struct EventLoopPtr evlp, int32_t fd);

cme_error_t EventLoopPtr_set_pollin(struct EventLoopPtr evlp, int32_t fd);

cme_error_t EventLoopPtr_start(struct EventLoopPtr evlp);

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
