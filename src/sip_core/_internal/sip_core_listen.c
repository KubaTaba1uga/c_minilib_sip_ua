/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/sip_core_listen.h"
#include "sip_core/_internal/sip_core_strans_map.h"
#include "sip_core/sip_core.h"
#include "sip_transport/sip_transport.h"
#include "stc/cstr.h"
#include "utils/generic_ptr.h"
#include "utils/sip_status_codes.h"

static cme_error_t __SipCore_sip_transp_recvh(struct SipMessagePtr sip_msg,
                                              struct IpAddrPtr peer_ip,
                                              struct SipTransportPtr sip_transp,
                                              struct GenericPtr arg);

cme_error_t __SipCore_listen(sip_core_connh_t connh,
                             struct GenericPtr connh_arg,
                             struct SipCorePtr sip_core) {
  cme_error_t err;

  queue__SipCoreListeners_push(sip_core.get->listeners,
                               (struct __SipCoreListener){
                                   .connh = connh,
                                   .connh_arg = connh_arg,
                               });

  err = SipTransportPtr_listen(sip_core.get->sip_transp,
                               __SipCore_sip_transp_recvh,
                               GenericPtr_from_arc(SipCorePtr, sip_core));
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t __SipCore_sip_transp_recvh(
    struct SipMessagePtr sip_msg, struct IpAddrPtr peer_ip,
    struct SipTransportPtr sip_transp, struct GenericPtr arg) {
  /*
    On every request we do:
     1. If it is sip request (which is not ACK and is not matching to any
current server tramsaction) we are creating new server transaction and running
user `connh` callback with this new transaction.

     2. If it is sip request which is matching to any current server tramsaction
we are running user `reqh` callback whith exsisting server transaction.

     3. If it is sip request ACK we are looking for server transaction that it
matches to to change it state to DONE. We are not running any user
callbacks.

     4. If it is sip status we are looking for client transaction that it
matches to. We then run callback for client_transaction rather than listener.
This means we need sth to match client transactions and user callbacks.
   */
  puts(__func__);
  struct SipCorePtr sip_core = GenericPtr_dump(SipCorePtr, arg);
  struct SipServerTransactionPtr strans = {0};
  cme_error_t err;

  assert(sip_core.get != NULL);

  /* If there are no listeners there is no point in processing the message. */
  if (queue__SipCoreListeners_is_empty(sip_core.get->listeners)) {
    return 0;
  }

  if (SipMessagePtr_is_request(sip_msg)) {
    csview branch = {0};

    void *result = SipMessagePtr_get_branch(sip_msg, &branch);
    if (!result) {
      err = cme_error(ENODATA, "Missing via->branch in sip msg, which is "
                               "required in server transaction request");
      goto error_out;
    }

    result =
        SipServerTransactions_find(branch, sip_core.get->stranses, &strans);
    if (!result) {
      puts("New server transaction");
      err = SipServerTransactionPtr_create(sip_msg, sip_core, peer_ip, &strans);
      if (err) {
        goto error_out;
      }

      err = SipServerTransactionPtr_recv_next_state(sip_msg, strans);
      if (err) {
        goto error_out;
      }

      c_foreach(lstner, queue__SipCoreListeners, *sip_core.get->listeners) {
        err =
            lstner.ref->connh(sip_msg, sip_core, strans, lstner.ref->connh_arg);
        if (err) {
          goto error_strans_cleanup;
        }

        // If TU accepted the transaction break further processing.
        // Currently transaction support only one TU ops, so we do
        // not want to allow for multiple accepts.
        if (strans.get->tu_ops.reqh && strans.get->tu_ops.errh) {
          break;
        }
      }
    } else {
      puts("Matched old server transaction");
      err = SipServerTransactionPtr_recv_next_state(sip_msg, strans);
      if (err) {
        goto error_out;
      }
    }

  } else {
    // TO-DO: handle client transaction
  }

  return 0;

error_strans_cleanup:
  SipServerTransactionPtr_drop(&strans);
error_out:
  return cme_return(err);
}

cme_error_t __SipCore_accept(struct SipCoreAcceptOps accept_ops,
                             struct SipServerTransactionPtr sip_strans) {
  csview strans_id = {0};
  cme_error_t err;

  err = SipServerTransactionPtr_get_id(sip_strans, &strans_id);
  if (err) {
    goto error_out;
  }

  err =
      SipServerTransactionPtr_reply(SIP_STATUS_OK, cstr_lit("Ok"), sip_strans);
  if (err) {
    goto error_out;
  }

  struct SipServerTransactionPtr result;
  err = SipServerTransactions_insert(
      strans_id, sip_strans, sip_strans.get->sip_core.get->stranses, &result);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t
__SipCore_reject_busy_here(struct SipServerTransactionPtr sip_strans) {
  csview strans_id = {0};
  cme_error_t err;

  err = SipServerTransactionPtr_get_id(sip_strans, &strans_id);
  if (err) {
    goto error_out;
  }

  err = SipServerTransactionPtr_reply(SIP_STATUS_BUSY_HERE,
                                      cstr_lit("Busy Here"), sip_strans);
  if (err) {
    goto error_out;
  }

  hmap__SipServerTransactions_erase(sip_strans.get->sip_core.get->stranses,
                                    strans_id.buf);

  return 0;

error_out:
  return cme_return(err);
};
