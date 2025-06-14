/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_STRANS_H
#define C_MINILIB_SIP_UA_INT_SIP_STRANS_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/common.h"
#include "sip_core/_internal/sip_strans_hashmap.h"
#include "sip_core/sip_core.h"
#include "sip_transp/sip_transp.h"
#include "timer/timer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

enum cmsu_SipStransState {
  cmsu_SipStransState_TRYING,     // Initial state before anything is sent
  cmsu_SipStransState_PROCEEDING, // After sending 100 Trying or 1xx provisional
  cmsu_SipStransState_COMPLETED,  // After sending 3xx–6xx final response
  cmsu_SipStransState_CONFIRMED,  // After receiving ACK for 3xx–6xx response
  cmsu_SipStransState_TERMINATED, // After sending 2xx final response or
                                  // completing ACK processing
};

struct cmsu_SipStrans {
  sip_msg_t request;
  sip_core_t core;
  enum cmsu_SipStransState state;
  bool is_invite;
  bool is_100_send;
  mytimer_t invite_100_timer;
};

static inline cme_error_t cmsu_SipStrans_create(sip_msg_t sip_msg,
                                                sip_core_t sip_core,
                                                struct cmsu_SipStrans **out) {
  /* Server transaction is co,posed of one or more SIP request and multiple SIP
   * statuses. This function is reposnible for creating new sip server
   * transaction. Once new transaction is created use match on new sip messages
   * to change it's state. */
  struct cmsc_String branch = {0};
  struct cmsc_String method = {0};
  cme_error_t err;

  struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&sip_msg->vias);
  if (via) {
    branch = cmsc_bs_msg_to_string(&via->branch, sip_msg);
  }

  if (!branch.len) {
    err = cme_error(ENODATA, "Missing via->branch in server transaction");
    goto error_out;
  }

  method = cmsc_bs_msg_to_string(&sip_msg->request_line.sip_method, sip_msg);
  if (!method.len) {
    err = cme_error(ENODATA,
                    "Missing request_line->method in server transaction");
    goto error_out;
  }

  sip_strans_t strans = calloc(1, sizeof(struct cmsu_SipStrans));
  if (!strans) {
    err = cme_error(ENOMEM, "Cannot alocate memory for server transaction");
    goto error_out;
  }

  bool is_invite = strncmp(method.buf, "INVITE", method.len) == 0;

  err = my_hmap_cmsu_SipStransMap_insert(branch.buf, branch.len, strans,
                                         &sip_core->sip_strans, out);
  if (err) {
    goto error_strans_cleanup;
  }

  strans->core = sip_core;
  strans->request = sip_msg;
  strans->is_invite = is_invite;
  strans->state = cmsu_SipStransState_PROCEEDING;

  *out = strans;

  return 0;

error_strans_cleanup:
  free(strans);
error_out:
  return cme_return(err);
};

static inline struct cmsu_SipStrans *cmsu_SipStrans_find(sip_msg_t sip_msg,
                                                         sip_core_t sip_core) {
  struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&sip_msg->vias);
  struct cmsc_String branch = {0};
  if (via) {
    branch = cmsc_bs_msg_to_string(&via->branch, sip_msg);
  }

  if (!branch.len) {
    goto error_out;
  }

  return my_hmap_cmsu_SipStransMap_find(branch.buf, branch.len,
                                        &sip_core->sip_strans);

error_out:
  return NULL;
}

static inline void cmsu_SipStrans_destroy(struct cmsu_SipStrans **out) {
  if (!out || !*out) {
    return;
  }

  free(*out);
  *out = NULL;
};

static inline bool cmsu_SipStrans_is_done(struct cmsu_SipStrans *strans) {
  return strans->state == cmsu_SipStransState_COMPLETED ||
         strans->state == cmsu_SipStransState_TERMINATED;
};

static inline cme_error_t cmsu_SipStrans_reply(uint32_t status_code,
                                               const char *status_msg,
                                               sip_msg_t received_msg,
                                               struct cmsu_SipStrans *strans) {

  sip_msg_t response_msg;
  cme_error_t err;

  err = cmsu_sip_msg_create_response_from_request(received_msg, &response_msg);
  if (err) {
    goto error_out;
  }

  // TO-DO: send actual message

  if (status_code == 100) {
    strans->is_100_send = true;
  }

  return 0;

  /* error_response_cleanup: */
  /* cmsc_sipmsg_destroy_with_buf(&response_msg); */

error_out:
  return cme_return(err);
}

static inline cme_error_t
cmsu_SipStrans_timer_timeouth_invite_100_timer(mytimer_t timer, void *data) {
  struct cmsu_SipStrans *strans = data;

  cmsu_SipStrans_reply(100, "Trying", strans->request, strans);

  puts("Hit cmsu_SipStrans_invite_timer_timeouth");

  return 0;
}

static inline bool cmsu_SipStrans_next_state(sip_msg_t sip_msg,
                                             struct cmsu_SipStrans *strans) {
  puts("Hit cmsu_SipStrans_next_state");

  switch (strans->state) {
  case cmsu_SipStransState_PROCEEDING: {
    struct cmsc_String sip_method =
        cmsc_bs_msg_to_string(&sip_msg->request_line.sip_method, sip_msg);

    if (strncmp(sip_method.buf, "INVITE", sip_method.len) == 0) {
      puts("Hit cmsu_SipStrans_next_state::invite");

      // send 100 if TU won't in 200ms
      mytimer_create(strans->core->evl, 0,
                     200000000, // 200ms
                     cmsu_SipStrans_timer_timeouth_invite_100_timer, strans,
                     &strans->invite_100_timer);
    }

    return true;
  }
  default:
    return true;
  }
};

#endif
