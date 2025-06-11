/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_H
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_core/sip_core.h"
#include "sip_transp/sip_transp.h"
#include "udp/udp.h"
#include "utils/ip.h"

struct cmsu_SipCore {
  sip_transp_t sip_transp;
};

static inline cme_error_t
cmsu_SipCore_create(event_loop_t evl, ip_t ip_addr,
                    enum SupportedSipTranspProtos sstp, sip_core_t *out) {
  struct cmsu_SipCore *sip_core = calloc(1, sizeof(struct cmsu_SipCore));
  cme_error_t err;

  if (!sip_core) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `sip_core`");
    goto error_out;
  }

  err = sip_transp_create(evl, ip_addr, sstp, &sip_core->sip_transp);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
