/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
#define C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "udp_socket/udp_socket.h"
#include "utils/ip.h"
#include "utils/sip_msg.h"

enum SipTransportProtocolType {
  SipTransportProtocolType_UDP,
};

struct SipTransportPtr;
typedef cme_error_t (*sip_transp_recvh_t)(struct SipMessagePtr sip_msg,
                                          struct IpAddrPtr peer_ip,
                                          struct SipTransportPtr sip_transpp,
                                          struct GenericPtr data);

struct __SipTransport {
  // Transp Protocol data
  enum SipTransportProtocolType proto_type;
  struct EventLoopPtr evl;

  // Udp socket data
  struct UdpSocketPtr udp_socket;

  // User data & ops
  sip_transp_recvh_t recvh;
  struct GenericPtr recvh_arg;
};

void __SipTransport_destroy(struct __SipTransport *sip_transp);

static inline struct __SipTransport
__SipTransport_clone(struct __SipTransport udp_socket) {
  return udp_socket;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
