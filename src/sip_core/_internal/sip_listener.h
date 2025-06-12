/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_LISTENER_H
#define C_MINILIB_SIP_UA_INT_SIP_LISTENER_H

#include "sip_core/sip_core.h"

struct cmsu_SipListener {
  sip_core_request_handler_t request_handler;

  void *arg;
};

#endif
