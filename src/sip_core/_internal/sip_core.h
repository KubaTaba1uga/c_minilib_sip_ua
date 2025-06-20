/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_H
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"
#include "utils/ip.h"

#include "sip_core/_internal/common.h"

#define i_type SipCorePtr
#define i_key struct __SipCore
#define i_keydrop __SipCore_destroy
#define i_keyclone __SipCore_clone
#include "stc/arc.h"

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_H
