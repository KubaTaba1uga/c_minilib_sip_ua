/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_listener.h"
#include "sip_core/_internal/sip_strans.h"
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
                                              void *data,
                                              struct cmsu_SipCore *sip_core) {
  cme_error_t err;
  void *out = list_cmsu_SipListeners_push_front(
      &sip_core->sip_lstnrs,
      (struct cmsu_SipListener){.request_handler = reqh, .arg = data});
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
  /*
    On every request we do:
     1. If it is sip request (which is not ACK and is not matching to any
current server tramsaction) we are creating new server transaction and running
user callback with this new transaction.

     2. If it is sip request which is matching to any current server tramsaction
we are running user callback whith exsisting server transaction.

     3. If it is sip request ACK we are looking for server transaction that it
matches to to change it state to DONE. We are not running any user
callbacks.

     4. If it is sip status we are looking for client transaction that it
matches to. We then run callback for client_transaction rather than listener.
This means we need sth to match client transactions and user callbacks.
   */

  struct cmsu_SipCore *sip_core = data;
  bool is_request = cmsc_sipmsg_is_field_present(
      sip_msg, cmsc_SupportedSipHeaders_REQUEST_LINE);
  cme_error_t err;

  if (is_request) {
    sip_strans_t strans = NULL;

    // we are ignoring return error on purpose
    strans = cmsu_SipStrans_find(sip_msg, sip_core);
    if (!strans) {
      err = cmsu_SipStrans_create(sip_msg, sip_core, &strans);

      if (err) {
        goto error_out;
      }
    }

    bool should_code_continue =
        cmsu_SipStrans_next_state(sip_msg, sip_core, strans);
    if (!should_code_continue) {
      goto out;
    }

    c_foreach(lstner, list_cmsu_SipListeners, sip_core->sip_lstnrs) {
      err = lstner.ref->request_handler(sip_msg, peer_ip, strans, sip_core,
                                        lstner.ref->arg);
      if (err) {
        goto error_out;
      }
    }
  } else {
    /* c_foreach(lstner, list_cmsu_SipListeners, sip_core->sip_lstnrs) { */
    /*   err = lstner.ref->response_handler(sip_msg, peer_ip, sip_core, */
    /*                                      lstner.ref->arg); */
    /*   if (err) { */
    /*     goto error_out; */
    /*   } */
    /* } */
  }

out:
  puts("Sip core received message");
  return 0;
error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
