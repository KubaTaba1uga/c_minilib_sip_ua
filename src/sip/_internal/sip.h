/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_STACK_H
#define C_MINILIB_SIP_UA_INT_SIP_STACK_H
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip/_internal/common.h"
#include "sip/_internal/sip_request.h"
#include "sip/_internal/sip_transaction.h"
#include "sip/_internal/sip_transaction_client_other.h"
#include "sip/_internal/sip_transaction_list.h"
#include "sip/sip.h"
#include "socket/socket.h"
#include "socket99.h"
#include "utils/id.h"

/******************************************************************************
 *                             Sip Stack                                    *
 ******************************************************************************/
static inline void cmsu_SipStack_destroy(void *out) {
  if (!out) {
    return;
  }

  struct cmsu_SipStack **sip_stack = out;
  if (!*sip_stack) {
    return;
  }

  socket_destroy(&(*sip_stack)->socket);
  free(*sip_stack);
  *sip_stack = NULL;
}

static inline cme_error_t cmsu_SipStack_recv_callback(socket_t socket,
                                                      ip_addr_t *sender,
                                                      buffer_t *buf,
                                                      void *ctx_);
static inline cme_error_t cmsu_SipStack_send_callback(socket_t socket,
                                                      ip_addr_t *recver,
                                                      buffer_t *buf, void *data,
                                                      void *ctx);
static inline cme_error_t cmsu_SipStack_recv_fail_callback(socket_t socket,
                                                           ip_addr_t *sender,
                                                           buffer_t *buf,
                                                           void *ctx_);
static inline cme_error_t
cmsu_SipStack_send_fail_callback(socket_t socket, ip_addr_t *recver,
                                 buffer_t *buf, void *data, void *ctx);

static inline struct cmsc_String
cmsu_SipTransaction_get_top_via_branch(struct cmsu_SipTransaction *siptrans);
static inline cme_error_t cmsu_SipStack_create(evl_t evl, ip_addr_t ipaddr,
                                               struct cmsu_SipStack **out) {
  struct cmsu_SipStack *sip_stack = calloc(1, sizeof(struct cmsu_SipStack));
  cme_error_t err;
  if (!sip_stack) {
    err = cme_error(ENOMEM, "Cannot allocate memory for ua");
    goto error_out;
  }

  // We are marking sip stack for receive. If one want to send sth socket_send
  //  will set appropriate events. In udp we need to always set at least receive
  //  flag because even after send we want to receive confirmation that packets
  //  were received.
  err = socket_udp_create(
      evl, ipaddr, SocketEvent_RECEIVE, sip_stack, cmsu_SipStack_recv_callback,
      cmsu_SipStack_recv_fail_callback, cmsu_SipStack_send_callback,
      cmsu_SipStack_send_fail_callback, cmsu_SipStack_destroy,
      &sip_stack->socket);
  if (err) {
    goto error_sip_stack_cleanup;
  }

  sip_stack->transactions = list_cmsu_SipTransactions_init();

  *out = sip_stack;

  return 0;

error_sip_stack_cleanup:
  free(sip_stack);
error_out:
  return cme_return(err);
}

static inline cme_error_t cmsu_SipStack_recv_callback(socket_t socket,
                                                      ip_addr_t *sender,
                                                      buffer_t *buf,
                                                      void *ctx_) {
  struct cmsu_SipStack *sip_stack = ctx_;
  struct cmsc_SipMessage *sipmsg;
  cme_error_t err;
  printf("Starting %s...\n", __func__);

  err = cmsc_parse_sip(buf->len, buf->buf, &sipmsg);
  if (err) {
    goto error_out;
  }

  c_foreach(siptrans, list_cmsu_SipTransactions, sip_stack->transactions) {
    struct cmsc_String local_branch =
        cmsu_SipTransaction_get_top_via_branch(siptrans.ref);
    struct cmsc_String remote_branch =
        cmsc_bs_msg_to_string(&sipmsg->vias.stqh_first->branch, sipmsg);

    if (strncmp(local_branch.buf, remote_branch.buf, local_branch.len) == 0) {
      switch (siptrans.ref->type) {
      case cmsu_SipportedSipTransactions_CLIENT_OTHER: {
        err = cmsu_SipTransactionClientOther_recv_callback(
            socket, sender, sipmsg, siptrans.ref->data, sip_stack);
        break;
      }
      default:;
        err = cme_errorf(ENODATA, "Unsupported transaction type: %d",
                         siptrans.ref->type);
      }
      if (err) {
        goto error_out;
      }
    }
  }

  struct cmsc_String call_id = cmsc_bs_msg_to_string(&sipmsg->call_id, sipmsg);
  printf("Received %.*s\n", call_id.len, call_id.buf);

  printf("Finished %s\n", __func__);

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t cmsu_SipStack_recv_fail_callback(socket_t socket,
                                                           ip_addr_t *sender,
                                                           buffer_t *buf,
                                                           void *ctx_) {
  puts("Hit sip recv fail");

  return 0;
}

static inline cme_error_t cmsu_SipStack_send_callback(socket_t socket,
                                                      ip_addr_t *recver,
                                                      buffer_t *buf, void *data,
                                                      void *ctx) {
  struct cmsu_SipTransaction *sip_trans = data;
  struct cmsu_SipStack *sip_stack = ctx;
  cme_error_t err;
  switch (sip_trans->type) {
  case cmsu_SipportedSipTransactions_CLIENT_OTHER: {
    err = cmsu_SipTransactionClientOther_send_callback(
        socket, recver, buf, sip_trans->data, sip_stack);
    break;
  }
  default:;
    err = cme_errorf(ENODATA, "Unsupported transaction type: %d",
                     sip_trans->type);
  }
  if (err) {
    goto error_out;
  }

  puts("Hit sip send success");
  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsu_SipStack_send_fail_callback(socket_t socket, ip_addr_t *recver,
                                 buffer_t *buf, void *data, void *ctx) {
  puts("Hit sip send fail");
  return 0;
};

id_t cmsu_SipStack_gen_id(sip_stack_t sip_stack) {
  return id_generate(&sip_stack->id_gen);
}

static inline cme_error_t
cmsu_SipStack_send_transaction(struct cmsu_SipTransaction *siptrans) {
  // This is the same as sipstack connect, we can provide appropriate callbacks
  // on this funcotion.
  cme_error_t err;
  switch (siptrans->type) {
  case cmsu_SipportedSipTransactions_CLIENT_OTHER: {
    err = cmsu_SipTransactionClientOther_send_async(siptrans->data);
    break;
  }
  default:;
    err =
        cme_errorf(ENODATA, "Unsupported transaction type: %d", siptrans->type);
  }

  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline struct cmsc_String
cmsu_SipTransaction_get_top_via_branch(struct cmsu_SipTransaction *siptrans) {
  switch (siptrans->type) {
  case cmsu_SipportedSipTransactions_CLIENT_OTHER: {
    return cmsu_SipTransactionClientOther_get_top_via_branch(siptrans->data);
  }

  default:
    return (struct cmsc_String){.buf = "", .len = 0};
  }
}

cme_error_t cmsu_SipStack_connect(sip_stack_t sip_stack, ip_addr_t recver,
                                  sip_msg_t msg) {
  struct cmsu_SipTransaction *sip_trans;
  cme_error_t err;

  if (cmsu_is_sipmsg_request(msg)) {
    if (strncmp(cmsc_bs_msg_to_string(&msg->request_line.sip_method, msg).buf,
                "INVITE", msg->request_line.sip_method.len) == 0) {
      err = cme_error(EINVAL,
                      "Sending INVITE messages is not supported at the moment");
      goto error_out;
    }
    if (strncmp(cmsc_bs_msg_to_string(&msg->request_line.sip_method, msg).buf,
                "MESSAGE", msg->request_line.sip_method.len) == 0) {
      err = cmsu_SipTransactionClientOther_create(msg, recver, sip_stack,
                                                  &sip_trans);
      if (err) {
        goto error_out;
      }

      return cmsu_SipStack_send_transaction(sip_trans);
    }

    err = cme_errorf(
        EINVAL, "Sending %.*s messages is not supported at the moment",
        msg->request_line.sip_method.len,
        cmsc_bs_msg_to_string(&msg->request_line.sip_method, msg).buf);
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

#endif
