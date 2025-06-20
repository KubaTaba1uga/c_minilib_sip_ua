/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_TRANSPORT_COMMON_H
#define C_MINILIB_SIP_UA_INT_SIP_TRANSPORT_COMMON_H

#include "event_loop/event_loop.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"

struct __SipTransport {
  // Transp Protocol data
  enum SupportedSipTranspProtos proto_type;
  event_loop_t evl;

  // Udp socket data
  udp_socket_t udp_socket;

  // User data & ops
  sip_transp_recvh_t recvh;
  void *recvh_arg;
};

static inline void __SipTransport_destroy(void *data);
static inline struct __SipTransport
__SipTransport_clone(struct __SipTransport udp_socket);

#define i_type __SipTransportPtr
#define i_key struct __SipTransport
#define i_keydrop __SipTransport_destroy
#define i_keyclone __SipTransport_clone
#include "stc/arc.h"

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSPORT_COMMON_H
