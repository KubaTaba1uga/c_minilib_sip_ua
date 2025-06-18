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

static inline cme_error_t
__SipTransport_create(event_loop_t evl, ip_t ip_addr,
                      enum SupportedSipTranspProtos proto_type,
                      sip_transp_t *out) {
  struct __SipTransport sip_transp = {0};
  cme_error_t err;

  switch (proto_type) {
  case SupportedSipTranspProtos_UDP:
    err = udp_socket_create(evl, ip_addr, &sip_transp.udp_socket);
    if (err) {
      goto error_out;
    }
    break;
  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_out;
  }

  struct __SipTransportPtr *sip_transpp =
      malloc(sizeof(struct __SipTransportPtr));
  if (!sip_transpp) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `sip_transp`");
    goto error_out;
  }

  sip_transp.proto_type = proto_type;
  sip_transp.evl = event_loop_ref(evl);

  *sip_transpp = __SipTransportPtr_from(sip_transp);

  *out = sip_transpp;

  return 0;

error_out:
  *out = NULL;
  return cme_return(err);
};

static inline void __SipTransport_destroy(void *data) {
  struct __SipTransport *sip_transp = data;
  udp_socket_deref(sip_transp->udp_socket);
  event_loop_deref(sip_transp->evl);
};

static inline struct __SipTransport
__SipTransport_clone(struct __SipTransport udp_socket) {
  return udp_socket;
};

static inline sip_transp_t __SipTransport_ref(sip_transp_t sip_transpp) {
  __SipTransportPtr_clone(*sip_transpp);

  return sip_transpp;
}

static inline void __SipTransport_deref(sip_transp_t sip_transpp) {
  int32_t usage_count = __SipTransportPtr_use_count(sip_transpp);

  __SipTransportPtr_drop(sip_transpp);

  // If usage count is 1 before drop it means it will be 0
  //  after drop but ptr holding usage count get freed on drop.
  if (usage_count <= 1) {
    free(sip_transpp);
  }
}

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
