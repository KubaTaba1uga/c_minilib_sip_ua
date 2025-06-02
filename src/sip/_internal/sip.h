/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIPSESS_H
#define C_MINILIB_SIP_UA_INT_SIPSESS_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip/_internal/sip_transaction_list.h"
#include "sip/sip.h"
#include "socket/socket.h"
#include "utils/id.h"

/******************************************************************************
 *                             Sip Session                                    *
 ******************************************************************************/
struct cmsu_SipSession {
  socket_t
      socket; // We need socket in case Sip Session need to schedule some io.

  list_cmsu_SipTransactions
      transactions; // Transactions are responsible for matching requests and
                    // responses into pairs.

  id_gen_t id_gen;
};

static inline void cmsu_SipSession_destroy(void *out) {
  if (!out) {
    return;
  }

  struct cmsu_SipSession **sipsess = out;
  if (!*sipsess) {
    return;
  }

  socket_destroy(&(*sipsess)->socket);
  free(*sipsess);
  *sipsess = NULL;
}

static inline cme_error_t cmsu_SipSession_recv_callback(socket_t socket,
                                                        ip_addr_t *sender,
                                                        buffer_t *buf,
                                                        void *ctx_);
static inline cme_error_t cmsu_SipSession_send_callback(socket_t socket,
                                                        ip_addr_t *recver,
                                                        buffer_t *buf,
                                                        void *data, void *ctx);
static inline cme_error_t cmsu_SipSession_recv_fail_callback(socket_t socket,
                                                             ip_addr_t *sender,
                                                             buffer_t *buf,
                                                             void *ctx_);
static inline cme_error_t
cmsu_SipSession_send_fail_callback(socket_t socket, ip_addr_t *recver,
                                   buffer_t *buf, void *data, void *ctx);

static inline cme_error_t cmsu_SipSession_create(evl_t evl, ip_addr_t ipaddr,

                                                 struct cmsu_SipSession **out) {
  struct cmsu_SipSession *sipsess = calloc(1, sizeof(struct cmsu_SipSession));
  cme_error_t err;
  if (!sipsess) {
    err = cme_error(ENOMEM, "Cannot allocate memory for ua");
    goto error_out;
  }

  // We are marking sip session for receive. If one want to send sth socket_send
  //  will set appropriate events. In udp we need to always set at least receive
  //  flag because even after send we want to receive confirmation over some
  //  socket.
  err = socket_udp_create(
      evl, ipaddr, SocketEvent_RECEIVE, sipsess, cmsu_SipSession_recv_callback,
      cmsu_SipSession_recv_fail_callback, cmsu_SipSession_send_callback,
      cmsu_SipSession_send_fail_callback, cmsu_SipSession_destroy,
      &sipsess->socket);
  if (err) {
    goto error_sipsess_cleanup;
  }

  sipsess->transactions = list_cmsu_SipTransactions_init();

  *out = sipsess;

  return 0;

error_sipsess_cleanup:
  free(sipsess);
error_out:
  return cme_return(err);
}

static inline cme_error_t cmsu_SipSession_recv_callback(socket_t socket,
                                                        ip_addr_t *sender,
                                                        buffer_t *buf,
                                                        void *ctx_) {
  /* struct cmsu_SipSession *ctx = ctx_; */
  struct cmsc_SipMessage *sipmsg;
  cme_error_t err;
  printf("Starting %s...\n", __func__);

  err = cmsc_parse_sip(buf->len, buf->buf, &sipmsg);
  if (err) {
    goto error_out;
  }

  struct cmsc_String call_id = cmsc_bs_msg_to_string(&sipmsg->call_id, sipmsg);
  printf("Received %.*s\n", call_id.len, call_id.buf);

  printf("Finished %s\n", __func__);

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t cmsu_SipSession_recv_fail_callback(socket_t socket,
                                                             ip_addr_t *sender,
                                                             buffer_t *buf,
                                                             void *ctx_) {
  puts("Hit sip recv fail");

  return 0;
}

static inline cme_error_t cmsu_SipSession_send_callback(socket_t socket,
                                                        ip_addr_t *recver,
                                                        buffer_t *buf,
                                                        void *data, void *ctx) {
  puts("Hit sip send success");
  return 0;
};

static inline cme_error_t
cmsu_SipSession_send_fail_callback(socket_t socket, ip_addr_t *recver,
                                   buffer_t *buf, void *data, void *ctx) {
  puts("Hit sip send fail");
  return 0;
};

id_t cmsu_SipSession_gen_id(sip_session_t sipsess) {
  return id_generate(&sipsess->id_gen);
}

#endif
