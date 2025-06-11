/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip_transp/_internal/sip_transp.h"
#include "sip_transp/sip_transp.h"
#include "udp/udp.h"
#include "utils/ip.h"

static inline cme_error_t cmsu_SipTransport_recvh(char *buffer,
                                                  int32_t buffer_len, ip_t peer,
                                                  void *sock, void *data);

static inline cme_error_t cmsu_SipTransport_listen(sip_transp_t sip_transp,
                                                   sip_transp_recvh_t recvh,
                                                   void *arg) {
  cme_error_t err;

  switch (sip_transp->proto_type) {
  case SupportedSipTranspProtos_UDP:
    err = udp_listen(sip_transp->socket, cmsu_SipTransport_recvh, sip_transp);
    if (err) {
      goto error_out;
    }

    break;
  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_out;
  }

  sip_transp->recvh = recvh;
  sip_transp->recvh_arg = arg;

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t cmsu_SipTransport_recvh(char *buffer,
                                                  int32_t buffer_len, ip_t peer,
                                                  void *sock, void *data) {
  struct cmsc_SipMessage *sip_msg;
  sip_transp_t sip_transp = data;
  cme_error_t err;

  err = cmsc_parse_sip(buffer_len, buffer, &sip_msg);
  if (err) {
    // TO-DO log that malformed sip msg send.
    if (err->code == EINVAL) {
      return 0;
    }
    goto error_out;
  }

  puts("Received data over SIP");

  err = sip_transp->recvh(sip_msg, peer, sip_transp, sip_transp->recvh_arg);
  if (err) {
    goto error_out;
  }

  (void)sip_transp;

  return 0;

error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
