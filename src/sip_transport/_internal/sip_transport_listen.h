/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/sip_transport.h"
#include "stc/cstr.h"
#include "udp_socket/udp_socket.h"
#include "utils/buffer.h"
#include "utils/ip.h"
#include "utils/sip_msg.h"

static cme_error_t __SipTransport_recvh(buffer_t bufptr, ip_t peer, void *sock,
                                        void *data);

static inline cme_error_t __SipTransport_listen(sip_transp_t *sip_transp,
                                                sip_transp_recvh_t recvh,
                                                void *arg) {
  cme_error_t err;

  switch ((*sip_transp->get)->proto_type) {
  case SupportedSipTranspProtos_UDP:
    err = udp_socket_listen((*sip_transp->get)->udp_socket,
                            __SipTransport_recvh, sip_transp);
    if (err) {
      goto error_out;
    }

    break;
  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_out;
  }

  (*sip_transp->get)->recvh = recvh;
  (*sip_transp->get)->recvh_arg = arg;

  return 0;

error_out:
  return cme_return(err);
}

static cme_error_t __SipTransport_recvh(buffer_t bufptr, ip_t peer, void *sock,
                                        void *data) {
  struct cmsc_SipMessage *sip_msg;
  sip_transp_t *sip_transp = data;
  cme_error_t err;

  cstr_view buf_view = cstr_getview(bufptr.get);
  err = cmsc_parse_sip(buf_view.size, buf_view.data, &sip_msg);
  if (err) {
    // TO-DO log that malformed sip msg send.
    if (err->code == EINVAL) {
      return 0;
    }
    goto error_out;
  }

  sip_msg_t sip_msg_ptr = sip_msg_ptr_from(sip_msg);

  puts("Received data over SIP");

  err =
      (*sip_transp->get)
          ->recvh(sip_msg_ptr, peer, sip_transp, (*sip_transp->get)->recvh_arg);
  if (err) {
    goto error_out;
  }

  (void)sip_transp;

  return 0;

error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
