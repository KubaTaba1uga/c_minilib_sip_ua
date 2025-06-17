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
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"
#include "utils/ip.h"

struct __SipTransport {

  // Transp Protocol data
  enum SupportedSipTranspProtos proto_type;
  event_loop_ptr_t evl_ptr;

  // Udp socket data
  udp_socket_ptr_t udp_socket_ptr;

  // User data & ops
  sip_transp_recvh_t recvh;
  void *recvh_arg;
};

static inline cme_error_t
__SipTransport_create(event_loop_ptr_t evl, ip_t ip_addr,
                      enum SupportedSipTranspProtos proto_type,
                      sip_transp_ptr_t *out) {
  struct __SipTransport *sip_transp = calloc(1, sizeof(struct __SipTransport));
  cme_error_t err;

  if (!sip_transp) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `sip_transp`");
    goto error_out;
  }

  switch (proto_type) {
  case SupportedSipTranspProtos_UDP:
    sip_transp->proto_type = SupportedSipTranspProtos_UDP;
    err = udp_socket_create(evl, ip_addr, &sip_transp->udp_socket_ptr);
    if (err) {
      goto error_sip_transp_cleanup;
    }
    break;
  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_sip_transp_cleanup;
  }

  sip_transp->evl_ptr = event_loop_ref(evl);
  *out = sip_transp_ptr_from(sip_transp);

  return 0;

error_sip_transp_cleanup:
  free(sip_transp);
error_out:
  *out = sip_transp_ptr_init();
  return cme_return(err);
};

static inline sip_transp_ptr_t __SipTransport_ref(sip_transp_ptr_t sip_transp) {
  return sip_transp_ptr_clone(sip_transp);
};

static inline void __SipTransport_deref(sip_transp_ptr_t *sip_transp) {
  sip_transp_ptr_drop(sip_transp);
};

void __sip_transp_ptr_destroy(__SipTransportPtr *sip_transp) {
  event_loop_deref(&(*sip_transp)->evl_ptr);
  udp_socket_deref(&(*sip_transp)->udp_socket_ptr);

  free(sip_transp);
};

__SipTransportPtr __sip_transp_ptr_clone(__SipTransportPtr sip_transp) {
  return sip_transp;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
