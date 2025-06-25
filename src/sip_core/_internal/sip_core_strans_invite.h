#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core_strans.h"
#include "utils/sip_status_codes.h"
#include <asm-generic/errno-base.h>
#include <stdint.h>

#define __SIP_CORE_STRANS_INVITE_T100 200000000 // 200ms
#define __SIP_CORE_STRANS_T1 500000000          // 500ms

static inline cme_error_t
SipServerTransactionPtr_timeouth_invite_100_timer(struct TimerFdPtr timer,
                                                  struct GenericPtr data);

static inline cme_error_t SipServerTransactionPtr_invite_next_state(
    struct SipMessagePtr sip_msg, struct SipServerTransactionPtr strans) {
  cme_error_t err;

  switch (strans.get->state) {

  case __SipServerTransactionState_NONE:
    if (!strans.get->invite_100_timer.get) {
      // send 100 if TU won't in 200ms
      SipServerTransactionPtr_clone(strans);
      err = TimerFdPtr_create(
          strans.get->sip_core.get->evl, 0, __SIP_CORE_STRANS_INVITE_T100,
          SipServerTransactionPtr_timeouth_invite_100_timer,
          GenericPtr_from_arc(SipServerTransactionPtr, strans),
          &strans.get->invite_100_timer);
      if (err) {
        goto error_out;
      }
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
SipServerTransactionPtr_timeouth_invite_100_timer(struct TimerFdPtr timer,
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

  return 0;

error_out:
  TimerFdPtr_rearm(timer); // If we failed to send 100, we need to try again.
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
