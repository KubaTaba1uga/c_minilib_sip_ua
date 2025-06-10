/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_transp/_internal/sip_transp.h"
#include "sip_transp/sip_transp.h"
#include "udp/udp.h"
#include "utils/ip.h"

static inline cme_error_t
cmsu_SipTransport_udp_recvh(char *buffer, int32_t buffer_len, ip_t peer,
                            udp_socket_t udp_sock, void *data);

static inline cme_error_t
cmsu_SipTransport_listen(sip_transp_t sip_transp,
                         enum SupportedSipTranspProtos sstp,
                         sip_transp_recvh_t recvh, void *arg) {
  cme_error_t err;

  switch (sip_transp->proto_type) {
  case SupportedSipTranspProtos_UDP:
    err =
        udp_listen(sip_transp->socket, cmsu_SipTransport_udp_recvh, sip_transp);
    if (err) {
      goto error_out;
    }

    sip_transp->recvh = recvh;
    sip_transp->recvh_arg = arg;

    break;
  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t
cmsu_SipTransport_udp_recvh(char *buffer, int32_t buffer_len, ip_t peer,
                            udp_socket_t udp_sock, void *data) {
  puts("Received sip message over UDP");
  return 0;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
