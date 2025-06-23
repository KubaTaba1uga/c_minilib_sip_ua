/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_TRANSP_SEND_H
#define C_MINILIB_SIP_UA_INT_SIP_TRANSP_SEND_H
#include <stdio.h>

#include "event_loop/event_loop.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"

cme_error_t __SipTransport_send(struct SipTransportPtr *sip_transpp,
                                ip_t ip_addr, struct BufferPtr bytes) {
  cme_error_t err;

  switch (sip_transpp->get->proto_type) {
  case SipTransportProtocolType_UDP: {
    err = UdpSocketPtr_send(sip_transpp->get->udp_socket, ip_addr, bytes);
    if (err) {
      goto error_out;
    }

  } break;

  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_SEND_H
