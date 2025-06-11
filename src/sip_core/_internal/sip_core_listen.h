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
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_listener.h"
#include "sip_core/sip_core.h"
#include "sip_transp/sip_transp.h"
#include "stc/common.h"
#include "udp/udp.h"
#include "utils/ip.h"
#include "utils/sipmsg.h"

static inline cme_error_t cmsu_SipCore_sip_transp_recvh(sip_msg_t sip_msg,
                                                        ip_t peer_ip,
                                                        sip_transp_t sip_transp,
                                                        void *data);

static inline cme_error_t cmsu_SipCore_listen(sip_core_request_handler_t reqh,
                                              sip_core_response_handler_t resph,
                                              void *data,
                                              struct cmsu_SipCore *sip_core) {
  cme_error_t err;

  void *out = list_cmsu_SipListeners_push_front(
      &sip_core->sip_lstnrs,
      (struct cmsu_SipListener){
          .response_handler = resph, .request_handler = reqh, .arg = data});
  if (!out) {
    err = cme_error(ENOMEM, "Cannot allocate memory for new sip listener");
    goto error_out;
  }

  err = sip_transp_listen(sip_core->sip_transp, cmsu_SipCore_sip_transp_recvh,
                          sip_core);

  if (err) {
    goto error_list_lstners_cleanup;
  }

  return 0;

error_list_lstners_cleanup:
  list_cmsu_SipListeners_pop_front(&sip_core->sip_lstnrs);
error_out:
  return cme_return(err);
};

static inline cme_error_t cmsu_SipCore_sip_transp_recvh(sip_msg_t sip_msg,
                                                        ip_t peer_ip,
                                                        sip_transp_t sip_transp,
                                                        void *data) {
  struct cmsu_SipCore *sip_core = data;
  bool is_request = cmsc_sipmsg_is_field_present(
      sip_msg, cmsc_SupportedSipHeaders_REQUEST_LINE);
  cme_error_t err;

  if (is_request) {
    c_foreach(lstner, list_cmsu_SipListeners, sip_core->sip_lstnrs) {
      err = lstner.ref->request_handler(sip_msg, peer_ip, sip_core,
                                        lstner.ref->arg);
      if (err) {
        goto error_out;
      }
    }
  } else {
    c_foreach(lstner, list_cmsu_SipListeners, sip_core->sip_lstnrs) {
      err = lstner.ref->response_handler(sip_msg, peer_ip, sip_core,
                                         lstner.ref->arg);
      if (err) {
        goto error_out;
      }
    }
  }

  puts("Sip core received message");
  return 0;
error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
