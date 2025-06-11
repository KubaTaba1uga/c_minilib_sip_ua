/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/sip_core.h"
#include "sip_transp/sip_transp.h"
#include "udp/udp.h"
#include "utils/ip.h"

static inline cme_error_t cmsu_SipCore_sip_transp_recvh(sip_msg_t sip_msg,
                                                        ip_t peer_ip,
                                                        sip_transp_t sip_transp,
                                                        void *data);

static inline cme_error_t cmsu_SipCore_listen(struct cmsu_SipCore *sip_core) {
  cme_error_t err;

  err = sip_transp_listen(sip_core->sip_transp, cmsu_SipCore_sip_transp_recvh,
                          sip_core);

  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t cmsu_SipCore_sip_transp_recvh(sip_msg_t sip_msg,
                                                        ip_t peer_ip,
                                                        sip_transp_t sip_transp,
                                                        void *data) {
  puts("Sip core received message");
  return 0;
}

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
