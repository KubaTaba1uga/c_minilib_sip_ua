#include <assert.h>
#include <stdio.h>

#include "sip_core/_internal/sip_core_strans.h"
#include "sip_core/_internal/sip_core_strans_map.h"
#include "sip_transport/sip_transport.h"
#include "timer_fd/_internal/timer_fd.h"
#include "timer_fd/timer_fd.h"
#include "utils/generic_ptr.h"
#include "utils/memory.h"

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
SipServerTransactionPtr_reply(uint32_t status_code, cstr status,
                              struct SipServerTransactionPtr *strans);
static inline cme_error_t
SipServerTransactionPtr_timer_timeouth_invite_100_timer(struct TimerFdPtr timer,
                                                        struct GenericPtr data);
static inline cme_error_t
SipServerTransactionPtr_timer_timeouth_invite_3xx_6xx_timer(
    struct TimerFdPtr timer, struct GenericPtr data);
static inline cme_error_t
SipServerTransactionPtr_timer_timeouth_invite_retransmission_timer(
    struct TimerFdPtr timer, struct GenericPtr data);

cme_error_t
SipServerTransactionPtr_create(struct SipMessagePtr sip_msg,
                               struct SipCorePtr sip_core,
                               struct IpAddrPtr last_peer_ip,
                               // We need to take ptr to ptr becuase memory to
                               // struct SipServerTransactionPtr that we want to
                               // pass to TimerFdPtr_create (in next_state) is
                               // allocated on the heap and owned by hmap.
                               struct SipServerTransactionPtr *out) {
  /*
    Server transaction is composed of one or more SIP request and multiple SIP
    statuses. This function is reposnible for creating new sip server
    transaction and matching all next requests within this transaction to it.
    Once new transaction is created use next on transaction to change it's
    state.
  */
  bool is_invite = false;
  csview branch = {0};
  csview method = {0};
  cme_error_t err;

  void *result = SipMessagePtr_get_branch(sip_msg, &branch);
  if (!result) {
    err =
        cme_error(ENODATA, "Missing via->branch in server transaction request");
    goto error_out;
  }

  result = SipServerTransactions_find(branch, sip_core.get->stranses);
  if (result) {
    out = result;
    IpAddrPtr_assign(&out->get->last_peer_ip,
                     &last_peer_ip); // We need to update last peer ip
                                     // to know where to send reply.
    return 0;
  }

  result = SipMessagePtr_get_method(sip_msg, &method);
  if (!result) {
    err = cme_error(
        ENODATA,
        "Missing request_line->sip_method in server transaction request");
    goto error_out;
  }

  is_invite = strncmp(method.buf, "INVITE", method.size) == 0;

  struct __SipServerTransaction *strans =
      my_calloc(1, sizeof(struct __SipServerTransaction));

  *strans = (struct __SipServerTransaction){
      .state = __SipServerTransactionState_TRYING,
      .request = SipMessagePtr_clone(sip_msg),
      .is_invite = is_invite,
      .sip_core = sip_core,
      .last_peer_ip = last_peer_ip,
  };

  err = SipServerTransactions_insert(branch,
                                     SipServerTransactionPtr_from_ptr(strans),
                                     sip_core.get->stranses, out);
  if (err) {
    goto error_stransp_cleanup;
  }

  return 0;

error_stransp_cleanup:
  SipServerTransactionPtr_drop(out);
error_out:
  return cme_return(err);
}

void __SipServerTransaction_destroy(void *data) {
  puts(__func__);
  struct SipServerTransactionPtr *sip_strans = data;

  SipMessagePtr_drop(&sip_strans->get->request);
};

struct __SipServerTransaction
__SipServerTransaction_clone(struct __SipServerTransaction sip_strans) {
  return sip_strans;
};

cme_error_t
SipServerTransactionPtr_next_state(struct SipMessagePtr sip_msg,
                                   struct SipServerTransactionPtr strans) {
  cme_error_t err;

  switch (strans.get->state) {

  case __SipServerTransactionState_TRYING: {
    if (strans.get->is_invite) {
      if (!strans.get->invite_100_timer.get) {
        // send 100 if TU won't in 200ms
        SipServerTransactionPtr_clone(strans);
        err = TimerFdPtr_create(
            strans.get->sip_core.get->evl, 0,
            200000000, // 200ms
            SipServerTransactionPtr_timer_timeouth_invite_100_timer,
            GenericPtr_from_arc(SipServerTransactionPtr, &strans),
            &strans.get->invite_100_timer);
        if (err) {
          goto error_out;
        }
      }
    } else {
      // TO-DO process NON-INVITE server transaction
    }

  } break;

  case __SipServerTransactionState_PROCEEDING: {
  } break;

  case __SipServerTransactionState_COMPLETED: {
  } break;

  case __SipServerTransactionState_CONFIRMED: {
  } break;

  case __SipServerTransactionState_TERMINATED: {
  } break;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t
SipServerTransactionPtr_timer_timeouth_invite_100_timer(
    struct TimerFdPtr timer, struct GenericPtr data) {
  puts(__func__);
  struct SipServerTransactionPtr strans =
      GenericPtr_dump(SipServerTransactionPtr, data);

  assert(strans.get != NULL);

  (void)SipServerTransactionPtr_reply;
  /* cme_error_t err; */
  /* switch (strans.get->state) { */
  /* case __SipServerTransactionState_TRYING: */
  /*   err = SipServerTransactionPtr_reply(100, cstr_from("Trying"), strans); */
  /*   if (err) { */
  /*     goto error_out; */
  /*   } */

  /* default:; */
  /* } */

  return 0;

  /* error_out: */
  /*   TimerFdPtr_rearm(*timer); // If we failed to send 100, we need to try
   * again. */
  /*   return cme_return(err); */
};

static inline cme_error_t
SipServerTransactionPtr_reply(uint32_t status_code, cstr status,
                              struct SipServerTransactionPtr *strans) {
  puts(__func__);
  struct BufferPtr bytes;
  struct SipMessagePtr sipmsg;
  cme_error_t err;

  err = SipMessagePtr_status_from_request(strans->get->request, status_code,
                                          status, &sipmsg);
  if (err) {
    goto error_out;
  }

  err = SipMessagePtr_generate(sipmsg, &bytes);
  if (err) {
    goto error_sip_msg_cleanup;
  }

  err = SipTransportPtr_send(&strans->get->sip_core.get->sip_transp,
                             strans->get->last_peer_ip, bytes);
  if (err) {
    goto error_sip_msg_cleanup;
  }

  switch (strans->get->state) {
    if (strans->get->is_invite) {
    case __SipServerTransactionState_TRYING:
      if (status_code >= 100 && status_code <= 199) {
        strans->get->state = __SipServerTransactionState_PROCEEDING;
      }
      if (status_code >= 200 && status_code <= 299) {
        strans->get->state = __SipServerTransactionState_TERMINATED;
      }
    case __SipServerTransactionState_PROCEEDING:
      if (status_code >= 200 && status_code <= 299) {
        strans->get->state = __SipServerTransactionState_TERMINATED;
      }
      if (status_code >= 300 && status_code <= 699) {
        strans->get->state = __SipServerTransactionState_COMPLETED;

        err = TimerFdPtr_create(
            strans->get->sip_core.get->evl, 0, __SIP_CORE_STRANS_T1,
            SipServerTransactionPtr_timer_timeouth_invite_3xx_6xx_timer,
            GenericPtr_from_arc(SipServerTransactionPtr, strans),
            &strans->get->invite_3xx_6xx_timer);
        if (err) {
          goto error_sip_msg_cleanup;
        }
      }
    case __SipServerTransactionState_COMPLETED:
      strans->get->state = __SipServerTransactionState_COMPLETED;

      err = TimerFdPtr_create(
          strans->get->sip_core.get->evl, 0, __SIP_CORE_STRANS_T1 * 64,
          SipServerTransactionPtr_timer_timeouth_invite_retransmission_timer,
          GenericPtr_from_arc(SipServerTransactionPtr, strans),
          &strans->get->invite_retransmission_timer);
      if (err) {
        goto error_sip_msg_cleanup;
      }
    case __SipServerTransactionState_TERMINATED:
      SipServerTransactionPtr_drop(strans);
    } else {
      // Add handling for non invite
    }

    break;
  default:;
  }

  SipMessagePtr_drop(&sipmsg);

  return 0;

error_sip_msg_cleanup:
  SipMessagePtr_drop(&sipmsg);
error_out:
  return cme_return(err);
};

static inline cme_error_t
SipServerTransactionPtr_timer_timeouth_invite_3xx_6xx_timer(
    struct TimerFdPtr timer, struct GenericPtr data) {
  return 0;
};

static inline cme_error_t
SipServerTransactionPtr_timer_timeouth_invite_retransmission_timer(
    struct TimerFdPtr timer, struct GenericPtr data) {
  return 0;
};
