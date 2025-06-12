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
#include "sip_core/_internal/sip_listener.h"
#include "sip_core/_internal/sip_listener_list.h"
#include "sip_core/_internal/sip_strans.h"
#include "sip_core/sip_core.h"
#include "sip_transp/sip_transp.h"
#include "udp/udp.h"
#include "utils/ip.h"

struct cmsu_SipCore {
  sip_transp_t sip_transp;
  list_cmsu_SipListeners sip_lstnrs;
  hmap_cmsu_SipStransMap sip_strans;
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

  sip_core->sip_lstnrs = list_cmsu_SipListeners_init();
  sip_core->sip_strans = hmap_cmsu_SipStransMap_init();

  *out = sip_core;

  return 0;

error_out:
  *out = NULL;
  return cme_return(err);
};

void cmsu_SipCore_destroy(sip_core_t *out) {

  if (!out || !*out) {
    return;
  }

  sip_transp_destroy(&(*out)->sip_transp);
  free(*out);
  *out = NULL;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
