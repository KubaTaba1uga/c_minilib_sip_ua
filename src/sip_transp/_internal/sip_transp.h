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
#include "sip_transp/sip_transp.h"
#include "udp/udp.h"
#include "utils/ip.h"

struct cmsu_SipTransport {
  // Transp Protocol data
  enum SupportedSipTranspProtos proto_type;
  void *socket;

  // User data & ops
  sip_transp_recvh_t recvh;
  void *recvh_arg;
};

static inline cme_error_t
cmsu_SipTransport_create(event_loop_t evl, ip_t ip_addr,
                         enum SupportedSipTranspProtos proto_type,
                         sip_transp_t *out) {
  struct cmsu_SipTransport *sip_transp =
      calloc(1, sizeof(struct cmsu_SipTransport));
  cme_error_t err;

  if (!sip_transp) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `sip_transp`");
    goto error_out;
  }

  switch (proto_type) {
  case SupportedSipTranspProtos_UDP:
    sip_transp->proto_type = SupportedSipTranspProtos_UDP;
    err = udp_create(evl, ip_addr, (udp_socket_t *)&sip_transp->socket);
    if (err) {
      goto error_sip_transp_cleanup;
    }
    break;
  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_sip_transp_cleanup;
  }

  *out = sip_transp;

  return 0;

error_sip_transp_cleanup:
  free(sip_transp);
error_out:
  *out = NULL;
  return cme_return(err);
};

void cmsu_SipTransport_destroy(sip_transp_t *out) {
  if (!out || !*out) {
    return;
  }

  switch ((*out)->proto_type) {
  case SupportedSipTranspProtos_UDP:
    udp_destroy((*out)->socket);
    break;
  default:;
  }

  free(*out);

  *out = NULL;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
