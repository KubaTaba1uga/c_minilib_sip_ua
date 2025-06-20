/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/common.h"
#include "sip_core/_internal/sip_core_strans_map.h"
#include "sip_core/sip_core.h"
#include "stc/cstr.h"
#include "timer_fd/timer_fd.h"
#include "utils/csview_ptr.h"
#include "utils/sip_msg.h"

static inline cme_error_t __SipCoreStrans_create(sip_msg_t sip_msg,
                                                 sip_core_t sip_core,
                                                 sip_strans_t *out) {
  /*
    Server transaction is composed of one or more SIP request and multiple SIP
    statuses. This function is reposnible for creating new sip server
    transaction. Once new transaction is created use next on transaction
    to change it's state.
  */
  bool is_invite = false;
  csview branch = {0};
  csview method = {0};
  cme_error_t err;

  void *result = sip_msg_get_branch(sip_msg, &branch);
  if (!result) {
    err =
        cme_error(ENODATA, "Missing via->branch in server transaction request");
    goto error_out;
  }

  *out = __SipCoreStransMap_find(branch, &sip_core->get->stranses);
  if (*out != NULL) {
    return 0;
  }

  result = sip_msg_get_method(sip_msg, &method);
  if (!result) {
    err = cme_error(
        ENODATA,
        "Missing request_line->sip_method in server transaction request");
    goto error_out;
  }

  is_invite = strncmp(method.buf, "INVITE", method.size) == 0;

  sip_strans_t sip_stransp = malloc(sizeof(struct __SipCoreStransPtr));
  if (!sip_stransp) {
    err = cme_error(ENOMEM, "Cannot alocate memory for server transaction");
    goto error_out;
  }

  *sip_stransp = __SipCoreStransPtr_from((struct __SipCoreStrans){
      .state = __SipCoreStransState_TRYING,
      .request = sip_msg_ref(sip_msg),
      .is_invite = is_invite,
      .sip_core = sip_core,
  });

  err = __SipCoreStransMap_insert(branch, sip_stransp, &sip_core->get->stranses,
                                  out);
  if (err) {
    goto error_stransp_cleanup;
  }

  return 0;

error_stransp_cleanup:
  __SipCoreStrans_deref(sip_stransp);
error_out:
  *out = NULL;
  return cme_return(err);
};

static inline cme_error_t
__SipCoreStrans_timer_timeouth_invite_100_timer(timer_fd_t timer, void *data);

static inline cme_error_t __SipCoreStrans_next_state(sip_msg_t sip_msg,
                                                     sip_strans_t strans) {
  switch (strans->get->state) {
  case __SipCoreStransState_TRYING: {
    if (strans->get->is_invite) {
      // send 100 if TU won't in 200ms
      timer_fd_create(strans->get->sip_core->get->evl, 0,
                      200000000, // 200ms
                      __SipCoreStrans_timer_timeouth_invite_100_timer, strans,
                      &strans->get->invite_100_timer);
    } else {
      // TO-DO process NON-INVITE server transaction
    }

  } break;

  case __SipCoreStransState_PROCEEDING: {
  } break;

  case __SipCoreStransState_COMPLETED: {
  } break;

  case __SipCoreStransState_CONFIRMED: {
  } break;

  case __SipCoreStransState_TERMINATED: {
  } break;
  }

  return 0;
}

static inline cme_error_t
__SipCoreStrans_reply(uint32_t status_code, cstr status, sip_strans_t strans);

static inline cme_error_t
__SipCoreStrans_timer_timeouth_invite_100_timer(timer_fd_t timer, void *data) {
  sip_strans_t strans = data;
  cme_error_t err;
  puts("Hit __SipCoreStrans_invite_100_timeouth");

  switch (strans->get->state) {
  case __SipCoreStransState_TRYING:
    err = __SipCoreStrans_reply(100, cstr_from("Trying"), strans);
    if (err) {
      goto error_out;
    }

  default:;
  }

  return 0;
error_out:
  return cme_return(err);
};

static inline cme_error_t
__SipCoreStrans_reply(uint32_t status_code, cstr status, sip_strans_t strans) {
  puts("Hit __SipCoreStrans_reply");
  csview_ptr_t bytes;
  sip_msg_t sipmsg;
  cme_error_t err;

  err = sip_msg_status_from_request(strans->get->request, status_code, status,
                                    &sipmsg);
  if (err) {
    goto error_out;
  }

  err = sip_msg_generate(sipmsg, &bytes);
  if (err) {
    goto error_out;
  }

  // TO-DO: send bytes via transport

  switch (strans->get->state) {
  case __SipCoreStransState_TRYING:
    strans->get->state = __SipCoreStransState_PROCEEDING;

  default:;
  }

  return 0;
error_out:
  return cme_return(err);
};

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_H
