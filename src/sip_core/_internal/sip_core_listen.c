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
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_core_listen.h"
#include "sip_core/_internal/sip_core_strans_map.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "sip_core/sip_core.h"
#include "sip_transport/sip_transport.h"
#include "stc/cstr.h"
#include "utils/generic_ptr.h"
#include "utils/sip_status_codes.h"

static cme_error_t __SipCore_sip_transp_recvh(struct SipMessagePtr sip_msg,
                                              struct IpAddrPtr peer_ip,
                                              struct SipTransportPtr sip_transp,
                                              struct GenericPtr arg);

cme_error_t __SipCore_listen(sip_core_reqh_t reqh, struct GenericPtr arg,
                             struct SipCorePtr sip_core) {
  cme_error_t err;

  queue__SipCoreListeners_push(sip_core.get->listeners,
                               (struct __SipCoreListener){
                                   .reqh = reqh,
                                   .arg = arg,
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
        __SipServerTransactions_find(branch, sip_core.get->stranses, &strans);
    if (!result) {
      puts("New server transaction");
      err = SipServerTransactionPtr_create(sip_msg, sip_core, peer_ip, NULL,
                                           (struct GenericPtr){0}, &strans);
      if (err) {
        goto error_out;
      }

      c_foreach(lstner, queue__SipCoreListeners, *sip_core.get->listeners) {
        // We need reqh so the user can reply to incoming message.
        err = lstner.ref->reqh(sip_msg, peer_ip, strans, lstner.ref->arg);
        if (err) {
          goto error_strans_cleanup;
        }

        // If TU replied we want to stop further processing
        if (!__SipServerTransactionPtr_is_responses_empty(strans)) {
          break;
        }
      }
    } else {
      puts("Matched old server transaction");
      err = __SipServerTransactionPtr_recvh(sip_msg, peer_ip, &strans);
      if (err) {
        goto error_out;
      }
    }
  } else {
    // TO-DO: handle NONINVITE transaction
    assert(false);
  }

  return 0;

error_strans_cleanup:
  SipServerTransactionPtr_drop(&strans);
error_out:
  return cme_return(err);
}
