#include "sip_core/_internal/common.h"

void __SipCoreStrans_destroy(void *data) {
  struct __SipCoreStrans *sip_strans = data;

  sip_msg_deref(&sip_strans->request);
};

struct __SipCoreStrans
__SipCoreStrans_clone(struct __SipCoreStrans sip_strans) {
  return sip_strans;
};

  /* How server transaction works for INVITE flow? */

  /*
   Server transaction starts in TRYING state. Point of this state is to send 100
   trying response to the calle. Once 100 is send transaction move to PROCEEDING
   state.

   In PROCEEDING state we need to ensure all responses send by TU are passed to
   transport layer. Once user send 2xx in PROCEEDING state transaction move to
   TERMINATED state. Once user send from 300 to 699 in PROCEEDING state
   transaction move to COMPLETED state, now timer G is set up for T1.

   In TERMINATED state transaction get destructed and all timers are stopped.

   In COMPLETED state timer H is set for T1*64. This timer retransmit last
   response until TERMINATED state is reached. If ACk is received in COMPLETED
   state transaction move to CONFIRMED state.

   In CONFIRMED state transaction absorbs any additional ACK without informing
   User. When CONFIRMED state is entered timer I is set up for T4. Once timer I
   fires up transaction move to TERMINATED state.
   */

#define __SIP_CORE_STRANS_T1 500000000 // 500ms

static inline cme_error_t
__SipCoreStrans_reply(uint32_t status_code, cstr status, sip_strans_t strans);
static inline cme_error_t
__SipCoreStrans_timer_timeouth_invite_100_timer(timer_fd_t timer, void *data);
static inline cme_error_t
__SipCoreStrans_timer_timeouth_invite_3xx_6xx_timer(timer_fd_t timer,
                                                    void *data);
static inline cme_error_t
__SipCoreStrans_timer_timeouth_invite_retransmission_timer(timer_fd_t timer,
                                                           void *data);

static inline cme_error_t __SipCoreStrans_create(sip_msg_t sip_msg,
                                                 sip_core_t sip_core,
                                                 ip_t last_peer_ip,
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
    (*out)->get->last_peer_ip = last_peer_ip; // We need to update last peer ip
                                              // to know where to send reply.
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
      .last_peer_ip = last_peer_ip,
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

static inline cme_error_t __SipCoreStrans_next_state(sip_msg_t sip_msg,
                                                     sip_strans_t strans) {
  cme_error_t err;

  switch (strans->get->state) {
  case __SipCoreStransState_TRYING: {
    if (strans->get->is_invite) {
      if (!strans->get->invite_100_timer) {
        // send 100 if TU won't in 200ms
        err = timer_fd_create(strans->get->sip_core->get->evl, 0,
                              200000000, // 200ms
                              __SipCoreStrans_timer_timeouth_invite_100_timer,
                              strans, &strans->get->invite_100_timer);
        if (err) {
          goto error_out;
        }
      }
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

error_out:
  return cme_return(err);
}

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
  trimer_fd_rearm(timer); // If we failed to send 100, we need to try again.
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
    goto error_sip_msg_cleanup;
  }

  err = sip_transp_send(strans->get->sip_core->get->sip_transp,
                        strans->get->last_peer_ip, bytes);
  if (err) {
    goto error_sip_msg_cleanup;
  }

  switch (strans->get->state) {
    if (strans->get->is_invite) {
    case __SipCoreStransState_TRYING:
      if (status_code >= 100 && status_code <= 199) {
        strans->get->state = __SipCoreStransState_PROCEEDING;
      }
      if (status_code >= 200 && status_code <= 299) {
        strans->get->state = __SipCoreStransState_TERMINATED;
      }
    case __SipCoreStransState_PROCEEDING:
      if (status_code >= 200 && status_code <= 299) {
        strans->get->state = __SipCoreStransState_TERMINATED;
      }
      if (status_code >= 300 && status_code <= 699) {
        strans->get->state = __SipCoreStransState_COMPLETED;

        err = timer_fd_create(
            strans->get->sip_core->get->evl, 0, __SIP_CORE_STRANS_T1,
            __SipCoreStrans_timer_timeouth_invite_3xx_6xx_timer, strans,
            &strans->get->invite_3xx_6xx_timer);
        if (err) {
          goto error_sip_msg_cleanup;
        }
      }
    case __SipCoreStransState_COMPLETED:
      strans->get->state = __SipCoreStransState_COMPLETED;

      err = timer_fd_create(
          strans->get->sip_core->get->evl, 0, __SIP_CORE_STRANS_T1 * 64,
          __SipCoreStrans_timer_timeouth_invite_retransmission_timer, strans,
          &strans->get->invite_retransmission_timer);
      if (err) {
        goto error_sip_msg_cleanup;
      }
    case __SipCoreStransState_TERMINATED:
      sip_strans_deref(strans);
    } else {
      // Add handling for non invite
    }

    break;
  default:;
  }

  sip_msg_deref(&sipmsg);

  return 0;

error_sip_msg_cleanup:
  sip_msg_deref(&sipmsg);
error_out:
  return cme_return(err);
};
