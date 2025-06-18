/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/common.h"
#include "sip_core/sip_core.h"
#include "utils/sip_msg.h"

static inline cme_error_t
__SipCoreStrans_create(sip_msg_t sip_msg, sip_core_t sip_core,
                       struct __SipCoreStransPtr *out) {
  /*
    Server transaction is composed of one or more SIP request and multiple SIP
    statuses. This function is reposnible for creating new sip server
    transaction. Once new transaction is created use next on transaction
    to change it's state.
  */
  csview branch = {0};
  cme_error_t err;

  void *result = sip_msg_get_branch(sip_msg, &branch);
  if (!result) {
    err = cme_error(ENODATA, "Missing via->branch in server transaction");
    goto error_out;
  }

  sip_strans_t sip_stransp = malloc(sizeof(struct __SipCoreStransPtr));
  if (!sip_stransp) {
    err = cme_error(ENOMEM, "Cannot alocate memory for server transaction");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t __SipCore_sip_transp_recvh(sip_msg_t sip_msg,
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
  struct __SipCorePtr *sip_core = data;
  cme_error_t err;

  /* bool is_request = cmsc_sipmsg_is_field_present( */
  /*     sip_msg.get, cmsc_SupportedSipHeaders_REQUEST_LINE); */

  c_foreach(lstner, queue__SipCoreListenersQueue, sip_core->get->listeners) {
    err = lstner.ref->request_handler(sip_msg, peer_ip, NULL, sip_core,
                                      lstner.ref->arg);
    if (err) {
      goto error_out;
    }
  }

  (void)data;

  return 0;

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
