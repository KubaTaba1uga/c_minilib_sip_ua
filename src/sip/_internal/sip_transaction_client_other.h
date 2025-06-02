/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#ifndef C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H
#define C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H

#include "c_minilib_error.h"
#include "sip/_internal/sip_request.h"
#include "sip/_internal/sip_request_vec.h"
#include "sip/_internal/sip_transaction.h"
#include "sip/sip.h"
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
};

static inline cme_error_t
cmsu_SipTransactionClientOther_create(sip_msg_t sipmsg, sip_session_t sipsess,
                                      struct cmsu_SipTransaction **out) {
  struct cmsu_SipTransactionClientOther *trans_c_other =
      calloc(1, sizeof(struct cmsu_SipTransactionClientOther));
  cme_error_t err;
  if (!trans_c_other) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `trans_c_other`");
    goto error_out;
  }

  trans_c_other->trans.id = sip_session_gen_id(sipsess);
  trans_c_other->trans.data = trans_c_other;

  return 0;

error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H
