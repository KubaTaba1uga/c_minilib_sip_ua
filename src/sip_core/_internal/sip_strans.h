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
#include <stdbool.h>
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
  sip_core_t core;
  enum cmsu_SipStransState state;
  bool is_invite;
};

static inline cme_error_t cmsu_SipStrans_create(sip_msg_t sip_msg,
                                                sip_core_t sip_core,
                                                struct cmsu_SipStrans **out) {
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
  if (is_invite) {
    // TO-DO: implement 1. timer.
  }

  *out = strans;

  return 0;

error_out:
  return cme_return(err);
};

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

  /* static inline cme_error_t */
  /* cmsu_SipStrans_handle_request(sip_msg_t sip_msg, hmap_cmsu_SipStransMap
   * *stmap, */
  /*                               struct cmsu_SipStrans *strans, */
  /*                               bool *is_meant_for_listener) { */

  /*   switch (strans->state) { */
  /*   case cmsu_SipStransState_COMPLETED: */
  /*     // TO-DO: Handle ACK. In completed state ACK should not reach TU. They
   * are */
  /*     // meant only for transaction. */
  /*     *is_meant_for_listener = false; */
  /*     strans->state = cmsu_SipStransState_CONFIRMED; */
  /*     break; */
  /*   default: */
  /*     *is_meant_for_listener = true; */
  /*     break; */
  /*   } */

  /*   return 0; */
  /* } */

  /* static inline sip_strans_t cmsu_SipStrans_find(sip_msg_t sip_msg, */
  /*                                                hmap_cmsu_SipStransMap
   * *stmap)
   * { */
  /*   struct cmsc_String branch = {0}; */

  /*   struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&sip_msg->vias); */
  /*   if (via) { */
  /*     branch = cmsc_bs_msg_to_string(&via->branch, sip_msg); */
  /*   } */

  /*   if (!branch.len) { */
  /*     goto error_out; */
  /*   } */

  /*   cstr tmp_cstr_key = */
  /*       cstr_from_sv((csview){.buf = branch.buf, .size = branch.len}); */

  /*   const char *tmp_str_key = cstr_toraw(&tmp_cstr_key); */

  /*   if (!hmap_cmsu_SipStransMap_contains(stmap, tmp_str_key)) { */
  /*     goto error_out; */
  /*   } */

  /*   struct cmsu_SipStrans *value = */
  /*       hmap_cmsu_SipStransMap_at_mut(stmap, tmp_str_key); */
  /*   if (!value) { */
  /*     goto error_out; */
  /*   } */

  /*   return value; */

  /* error_out: */
  /*   return NULL; */
  /* } */

#endif
