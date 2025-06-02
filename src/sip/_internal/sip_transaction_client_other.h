/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#ifndef C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H
#define C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H

#include "c_minilib_error.h"
#include "sip/_internal/sip_request.h"
#include "sip/_internal/sip_response_vec.h"
#include "sip/_internal/sip_transaction.h"
#include "sip/sip.h"
#include "socket/socket.h"
#include "utils/id.h"
#include <asm-generic/errno-base.h>
#include <stdlib.h>

/*
 *  According RFC 3261 17.1.2:
 *
 *
 */

struct cmsu_SipTransactionClientOther {
  struct cmsu_SipTransaction trans;
  struct vec_cmsu_SipResponses responses;
  struct cmsu_SipRequest request;
};

static inline cme_error_t
cmsu_SipTransactionClientOther_create(sip_msg_t sipmsg, ip_addr_t recver,
                                      sip_stack_t sipstack,
                                      struct cmsu_SipTransaction **out) {
  struct cmsu_SipTransactionClientOther *trans_c_other =
      calloc(1, sizeof(struct cmsu_SipTransactionClientOther));
  cme_error_t err;
  if (!trans_c_other) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `trans_c_other`");
    goto error_out;
  }

  // Sip Request
  trans_c_other->request.id = sip_stack_gen_id(sipstack);
  trans_c_other->request.stack = sipstack;
  trans_c_other->request.recver = recver;
  trans_c_other->request.msg = sipmsg;

  // Sip Transaction
  trans_c_other->trans.type = cmsu_SipportedSipTransactions_CLIENT_OTHER;
  trans_c_other->trans.responses = vec_cmsu_SipResponses_init();
  trans_c_other->trans.id = sip_stack_gen_id(sipstack);
  trans_c_other->trans.data = trans_c_other;

  err = cmsu_SipStack_send_transaction(trans_c_other);
  if (err) {
    goto error_trans_cleanup;
  }
  *out = &trans_c_other->trans;

  return 0;

error_trans_cleanup:
  free(trans_c_other);
error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H
