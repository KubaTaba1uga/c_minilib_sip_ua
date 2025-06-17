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
#include "utils/smart_ptr.h"

static cme_error_t __SipTransport_udp_recvh(buffer_ptr_t bufptr, ip_t peer,
                                            void *data);

static inline cme_error_t
__SipTransport_listen(sip_transp_ptr_t *sip_transp_ptr,
                      sip_transp_recvh_t recvh, void *arg) {
  struct __SipTransport *sip_transp = SP_UNWRAP(*sip_transp_ptr);
  cme_error_t err;

  // Each transport proto needs seperate handler
  switch (sip_transp->proto_type) {
  case SupportedSipTranspProtos_UDP:
    err = udp_socket_listen(&sip_transp->udp_socket_ptr,
                            __SipTransport_udp_recvh, sip_transp_ptr);
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

static cme_error_t __SipTransport_udp_recvh(buffer_ptr_t bufptr, ip_t peer_ip,
                                            void *data) {
  sip_transp_ptr_t *sip_transp_ptr = data;
  struct cmsc_SipMessage *sip_msg;
  sip_transp_recvh_t *recvh;
  void *recvh_arg;
  cme_error_t err;

  recvh = SP_GET_PTR(*sip_transp_ptr, recvh);
  recvh_arg = SP_GET_PTR(*sip_transp_ptr, recvh_arg);

  cstr_view buf_view = cstr_getview(bufptr.get);
  err = cmsc_parse_sip(buf_view.size, buf_view.data, &sip_msg);
  if (err) {
    // TO-DO log that malformed sip msg send.
    if (err->code == EINVAL) {
      return 0;
    }
    goto error_out;
  }

  sip_msg_ptr_t sip_msg_ptr = sip_msg_ptr_from(sip_msg);

  puts("Received data over SIP");

  err = (*recvh)(sip_msg_ptr, peer_ip, sip_transp_ptr, recvh_arg);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_LISTEN_H
