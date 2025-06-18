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
#include "sip_core/_internal/common.h"
#include "sip_core/_internal/sip_core_listen.h"
#include "sip_core/sip_core.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"
#include "utils/ip.h"

static inline cme_error_t
__SipCore_create(event_loop_t evl, ip_t ip_addr,
                 enum SupportedSipTranspProtos proto_type, sip_core_t *out) {
  struct __SipCorePtr *sip_corep = malloc(sizeof(struct __SipCorePtr));
  struct __SipCore sip_core = {0};
  cme_error_t err;

  if (!sip_corep) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `struct __SipCorePtr`");
    goto error_out;
  }

  err = sip_transp_create(evl, ip_addr, proto_type, &sip_core.sip_transp);
  if (err) {
    goto error_corep_cleanup;
  }

  err = sip_transp_listen(sip_core.sip_transp, __SipCore_sip_transp_recvh,
                          sip_corep);
  if (err) {
    goto error_corep_cleanup;
  }

  sip_core.evl = event_loop_ref(evl);
  sip_core.listeners = queue__SipCoreListenersQueue_init();

  *sip_corep = __SipCorePtr_from(sip_core);

  *out = sip_corep;

  return 0;

error_corep_cleanup:
  __SipCorePtr_drop(sip_corep);
error_out:
  *out = NULL;
  return cme_return(err);
};

static inline sip_core_t __SipCore_ref(sip_core_t sip_corep) {
  __SipCorePtr_clone(*sip_corep);

  return sip_corep;
}

static inline void __SipCore_deref(sip_core_t sip_corep) {
  int32_t usage_count = __SipCorePtr_use_count(sip_corep);

  __SipCorePtr_drop(sip_corep);

  // If usage count is 1 before drop it means it will be 0
  //  after drop but ptr holding usage count get freed on drop.
  if (usage_count <= 1) {
    free(sip_corep);
  }
}

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_H
