/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#ifndef C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H
#define C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip/_internal/common.h"
#include "sip/_internal/sip_request.h"
#include "sip/_internal/sip_response.h"
#include "sip/_internal/sip_response_vec.h"
#include "sip/_internal/sip_transaction.h"
#include "sip/sip.h"
#include "socket/socket.h"
#include "utils/id.h"
#include <errno.h>
#include <stdio.h>
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
  struct cmsu_SipTransactionClientOther *siptrans =
      calloc(1, sizeof(struct cmsu_SipTransactionClientOther));
  cme_error_t err;
  if (!siptrans) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `siptrans`");
    goto error_out;
  }

  // Sip Request
  siptrans->request.id = sip_stack_gen_id(sipstack);
  siptrans->request.stack = sipstack;
  siptrans->request.recver = recver;
  siptrans->request.msg = sipmsg;

  // Sip Transaction
  siptrans->trans.type = cmsu_SipportedSipTransactions_CLIENT_OTHER;
  siptrans->trans.id = sip_stack_gen_id(sipstack);
  siptrans->trans.data = siptrans;
  siptrans->responses = vec_cmsu_SipResponses_init();

  *out = &siptrans->trans;

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t cmsu_SipTransactionClientOther_send_async(
    struct cmsu_SipTransactionClientOther *siptrans) {
  cme_error_t err;
  err = socket_send_async(siptrans->request.stack->socket,
                          &siptrans->request.recver, &siptrans->trans);
  if (err) {
    goto error_out;
  }

  struct cmsu_SipTransaction *result = list_cmsu_SipTransactions_push(
      &siptrans->request.stack->transactions, siptrans->trans);
  if (!result) {
    err = cme_error(ENOMEM, "Cannot add new transaction to sip stack");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t cmsu_SipTransactionClientOther_send_callback(
    socket_t socket, ip_addr_t *recver, buffer_t *buf,
    struct cmsu_SipTransactionClientOther *siptrans,
    struct cmsu_SipStack *sipstack) {
  cme_error_t err;

  err = cmsc_generate_sip(siptrans->request.msg, &buf->len,
                          (const char **)&buf->buf);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline struct cmsc_String
cmsu_SipTransactionClientOther_get_top_via_branch(
    struct cmsu_SipTransactionClientOther *siptrans) {
  return cmsc_bs_msg_to_string(&siptrans->request.msg->vias.stqh_first->branch,
                               siptrans->request.msg);
}

static inline cme_error_t cmsu_SipTransactionClientOther_recv_callback(
    socket_t socket, ip_addr_t *sender, sip_msg_t sipmsg,
    struct cmsu_SipTransactionClientOther *sip_trans, sip_stack_t sip_stack) {
  struct cmsu_SipResponse *result = vec_cmsu_SipResponses_push(
      &sip_trans->responses,
      (struct cmsu_SipResponse){
          .id = sip_stack_gen_id(sip_stack), .msg = sipmsg, .sender = *sender});
  cme_error_t err;
  if (!result) {
    err = cme_error(ENOMEM, "Cannot add response to sip transaction");
    goto error_out;
  }

  puts("Request and response matched!");

  return 0;

error_out:
  return cme_return(err);
}

/* static inline cme_error_t cmsu_SipTransactionClientOther_get_sipstack( */
/*     struct cmsu_SipTransactionClientOther *siptrans, void *data) */

#endif // C_MINILIB_SIP_UA_SIP_TRANSACTION_CLIENT_OTHER_H
