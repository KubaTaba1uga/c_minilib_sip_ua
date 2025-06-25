#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core_strans.h"
#include "timer_fd/_internal/timer_fd.h"
#include "utils/sip_msg.h"
#include "utils/sip_status_codes.h"
#include <asm-generic/errno-base.h>
#include <stdint.h>

#define __SIP_CORE_STRANS_INVITE_T100 200000000 // 200ms
#define __SIP_CORE_STRANS_T1 500000000          // 500ms

static inline cme_error_t
SipServerTransactionPtr_timeouth_invite_trying_timer(struct TimerFdPtr timer,
                                                     struct GenericPtr data);

static inline cme_error_t SipServerTransactionPtr_invite_next_state(
    struct SipMessagePtr sip_msg, struct SipServerTransactionPtr strans) {
  struct csview sip_method = {0};
  cme_error_t err;

  SipMessagePtr_get_method(sip_msg, &sip_method);
  if (csview_equals(sip_method, "INVITE")) {
    // If we have re-invite it means that this transaction was accepted before
    //   and another TU propably didn't receive 100. So we need to rearm our
    //   timer again.
    strans.get->state = __SipServerTransactionState_NONE;
    err = TimerFdPtr_create(
        strans.get->sip_core.get->evl, 0, __SIP_CORE_STRANS_INVITE_T100,
        SipServerTransactionPtr_timeouth_invite_trying_timer,
        GenericPtr_from_arc(SipServerTransactionPtr, strans),
        &strans.get->invite_trying_timer);
    if (err) {
      goto error_out;
    }
  }

  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    if (!strans.get->invite_trying_timer.get) {
      /*
        Send 100 if TU won't in 200ms. Once 100 is send move to PROCEEDING
        state.
      */
      SipServerTransactionPtr_clone(strans);
    }
    break;

  /* case __SipServerTransactionState_INVITE_PROCEEDING: { */
  /* } break; */

  /* case __SipServerTransactionState_INVITE_COMPLETED: { */
  /* } break; */

  /* case __SipServerTransactionState_INVITE_CONFIRMED: { */
  /* } break; */

  /* case __SipServerTransactionState_INVITE_TERMINATED: { */
  /* } break; */
  default:
    err = cme_errorf(EINVAL, "Unsupported invite server transaction state: %d",
                     strans.get->state);
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t
SipServerTransactionPtr_timeouth_invite_trying_timer(struct TimerFdPtr timer,
                                                     struct GenericPtr data) {
  puts(__func__);
  struct SipServerTransactionPtr strans =
      GenericPtr_dump(SipServerTransactionPtr, data);

  assert(strans.get != NULL);

  (void)SipServerTransactionPtr_reply;
  cme_error_t err;
  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    err = SipServerTransactionPtr_reply(SIP_STATUS_TRYING, cstr_from("Trying"),
                                        strans);
    if (err) {
      goto error_out;
    }

  default:;
  }

  // If we send 100, we need to stop sending another ones. If user
  // won't receive 100, user will send invite once again.
  strans.get->invite_trying_timer = (struct TimerFdPtr){0};

  return 0;

error_out:
  TimerFdPtr_rearm(                     // If we failed to send 100,
      strans.get->invite_trying_timer); // we need to try again.

  return cme_return(err);
};

static inline cme_error_t
SipServerTransactionPtr_timeouth_invite_3xx_6xx_timer(struct TimerFdPtr timer,
                                                      struct GenericPtr data) {
  return 0;
};

static inline cme_error_t
SipServerTransactionPtr_timeouth_invite_retransmission_timer(
    struct TimerFdPtr timer, struct GenericPtr data) {
  return 0;
};

cme_error_t SipServerTransactionPtr_invite_reply_callback(
    struct SipMessagePtr sip_msg, struct SipServerTransactionPtr strans) {
  uint32_t status_code = sip_msg.get->sip_msg->status_line.status_code;
  cme_error_t err;

  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    if (status_code >= SIP_STATUS_TRYING &&
        status_code <= SIP_STATUS_EARLY_DIALOG_TERMINATED) {
      strans.get->state = __SipServerTransactionState_INVITE_PROCEEDING;
    }
    if (status_code >= SIP_STATUS_OK &&
        status_code < SIP_STATUS_MULTIPLE_CHOICES) {
      strans.get->state = __SipServerTransactionState_INVITE_TERMINATED;
    }
    break;

  case __SipServerTransactionState_INVITE_PROCEEDING:
    if (status_code >= SIP_STATUS_TRYING &&
        status_code <= SIP_STATUS_EARLY_DIALOG_TERMINATED) {
      strans.get->state = __SipServerTransactionState_INVITE_TERMINATED;
    }
    if (status_code >= SIP_STATUS_MULTIPLE_CHOICES) {
      strans.get->state = __SipServerTransactionState_INVITE_COMPLETED;

      err = TimerFdPtr_create(
          strans.get->sip_core.get->evl, 0, __SIP_CORE_STRANS_T1,
          SipServerTransactionPtr_timeouth_invite_3xx_6xx_timer,
          GenericPtr_from_arc(SipServerTransactionPtr, strans),
          &strans.get->invite_3xx_6xx_timer);
      if (err) {
        goto error_out;
      }
    }
    break;

  default:
    err = cme_errorf(EINVAL, "Unsupported invite server transaction state: %d",
                     strans.get->state);
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}
