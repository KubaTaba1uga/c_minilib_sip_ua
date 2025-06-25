#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/sip_core_strans.h"
#include "stc/cstr.h"
#include "timer_fd/_internal/timer_fd.h"
#include "utils/bstring.h"
#include "utils/sip_msg.h"
#include "utils/sip_status_codes.h"
#include <asm-generic/errno-base.h>
#include <stdbool.h>
#include <stdint.h>

#define __SIP_CORE_STRANS_INVITE_T100 200000000 // 200ms
#define __SIP_CORE_STRANS_T1 500000000          // 500ms

static inline cme_error_t SipServerTransactionPtr_invite_next_state(
    struct SipMessagePtr sip_msg, struct SipServerTransactionPtr strans) {
  cme_error_t err;

  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    /*
      According rfc 3261 17.2.1 INVITE Server Transaction:
        When a server transaction is constructed for a request, it enters the
        "Proceeding" state. The server transaction MUST generate a 100
        (Trying) response ...
    */
    err = SipServerTransactionPtr_internal_reply(SIP_STATUS_TRYING,
                                                 cstr_from("Trying"), strans);
    if (err) {
      goto error_out;
    }

    strans.get->state = __SipServerTransactionState_INVITE_PROCEEDING;

    break;

  case __SipServerTransactionState_INVITE_PROCEEDING:
    /*
      According rfc 3261 17.2.1 INVITE Server Transaction:
        If a request retransmission is received while in the "Proceeding" state,
        the most recent provisional response that was received from the TU MUST
        be passed to the transport layer for retransmission ...
    */
    {
      struct csview reason_phrase;
      uint32_t status_code;

      void *result = SipMessagePtr_get_status_code_and_reason(
          strans.get->last_response, &status_code, &reason_phrase);
      if (!result) {
        err = cme_error(ENODATA, "No reason phrase and status code in sipmsg");
        goto error_out;
      }

      err = SipServerTransactionPtr_internal_reply(
          status_code, cstr_from_sv(reason_phrase), strans);
      if (err) {
        goto error_out;
      }
    }

    break;

  case __SipServerTransactionState_INVITE_COMPLETED:
    /* According rfc 3261 17.2.1 INVITE Server Transaction:
         If an ACK is received while the server transaction is in the
         "Completed" state, the server transaction MUST transition to the
         "Confirmed" state. As Timer G is ignored in this state, any
         retransmissions of the response will cease.
    */
    // TO-DO handle COMPLETED state ACK
    break;
  default:
    err = cme_errorf(EINVAL,
                     "Cannot handle invite server transaction in state: %d",
                     strans.get->state);
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t SipServerTransactionPtr_invite_tu_reply_next_state(
    uint32_t status_code, cstr status, struct SipServerTransactionPtr strans,
    bool *is_for_transp) {
  /*
    According rfc 3261 17.2.1 INVITE Server Transaction:
      The TU passes any number of provisional responses to the server
       transaction. So long as the server transaction is in the "Proceeding"
       state, each of these MUST be passed to the transport layer for
       transmission. They are not sent reliably by the transaction layer (they
       are not retransmitted by it) and do not cause a change in the state of
       the server transaction ...

      If, while in the "Proceeding" state, the TU passes a 2xx response to the
       server transaction, the server transaction MUST pass this response to the
       transport layer for transmission. It is not retransmitted by the server
       transaction; retransmissions of 2xx responses are handled by the TU. The
       server transaction MUST then transition to the "Terminated" state ...

      While in the "Proceeding" state, if the TU passes a response with status
       code from 300 to 699 to the server transaction, the response MUST be
       passed to the transport layer for transmission, and the state machine
       MUST enter the "Completed" state. For unreliable transports, timer G is
       set to fire in T1 seconds, and is not set to fire for reliable transports
       ...

    So as you can see TU sending reply changes server transaction state.
  */
  cme_error_t err;
  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    err = cme_error(EINVAL,
                    "Cannot reply using uninitialized sip server transaction");
    goto error_out;
  case __SipServerTransactionState_INVITE_PROCEEDING:
    *is_for_transp = true;

    if (status_code >= 200 && status_code < 300) {
      strans.get->state = __SipServerTransactionState_INVITE_TERMINATED;
    }
    if (status_code >= 200 && status_code < 300) {
      strans.get->state = __SipServerTransactionState_INVITE_COMPLETED;
      // TO-DO set unrialible transport timer G
    }
    break;
  default:
    *is_for_transp = false;
  }

  return 0;

error_out:
  return cme_return(err);
}

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

/* cme_error_t SipServerTransactionPtr_invite_reply_callback( */
/*     struct SipMessagePtr sip_msg, struct SipServerTransactionPtr strans, */
/*     bool *is_meant_for_transport) { */
/*   uint32_t status_code = sip_msg.get->sip_msg->status_line.status_code; */
/*   cme_error_t err; */

/*   switch (strans.get->state) { */
/*   case __SipServerTransactionState_NONE: */
/*     if (status_code >= SIP_STATUS_TRYING && */
/*         status_code <= SIP_STATUS_EARLY_DIALOG_TERMINATED) { */
/*       strans.get->state = __SipServerTransactionState_INVITE_PROCEEDING; */
/*     } */
/*     break; */

/*   case __SipServerTransactionState_INVITE_PROCEEDING: */
/*     if (status_code >= SIP_STATUS_TRYING && */
/*         status_code <= SIP_STATUS_EARLY_DIALOG_TERMINATED) { */
/*       strans.get->state = __SipServerTransactionState_INVITE_TERMINATED; */
/*     } */
/*     if (status_code >= SIP_STATUS_MULTIPLE_CHOICES) { */
/*       strans.get->state = __SipServerTransactionState_INVITE_COMPLETED; */

/*       err = TimerFdPtr_create( */
/*           strans.get->sip_core.get->evl, 0, __SIP_CORE_STRANS_T1, */
/*           SipServerTransactionPtr_timeouth_invite_3xx_6xx_timer, */
/*           GenericPtr_from_arc(SipServerTransactionPtr, strans), */
/*           &strans.get->invite_3xx_6xx_timer); */
/*       if (err) { */
/*         goto error_out; */
/*       } */
/*     } */
/*     break; */

/*   default: */
/*     err = cme_errorf(EINVAL, "Unsupported invite server transaction state:
 * %d", */
/*                      strans.get->state); */
/*     goto error_out; */
/*   } */

/*   return 0; */

/* error_out: */
/*   return cme_return(err); */
/* } */
