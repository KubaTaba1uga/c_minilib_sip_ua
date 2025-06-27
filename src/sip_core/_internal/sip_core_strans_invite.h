#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_core_strans.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/sip_transport.h"
#include "stc/cstr.h"
#include "timer_fd/_internal/timer_fd.h"
#include "timer_fd/timer_fd.h"
#include "utils/bstring.h"
#include "utils/generic_ptr.h"
#include "utils/sip_msg.h"
#include "utils/sip_status_codes.h"
#include <asm-generic/errno-base.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define __SIP_CORE_STRANS_INVITE_T100 200000000 // 200ms
#define __SIP_CORE_STRANS_T1 500000000          // 500ms

static inline cme_error_t
SipServerTransactionPtr_g_unrliable_timeouth(struct TimerFdPtr timer,
                                             struct GenericPtr data);

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

    So as you can see TU sending reply changes server transaction state.
  */
  cme_error_t err;
  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    err = cme_error(EINVAL,
                    "Cannot reply using uninitialized sip server transaction");
    goto error_out;
  case __SipServerTransactionState_INVITE_PROCEEDING:
    /*
      According rfc 3261 17.2.1 INVITE Server Transaction:
       The TU passes any number of provisional responses to the server
       transaction. So long as the server transaction is in the "Proceeding"
       state, each of these MUST be passed to the transport layer for
       transmission. They are not sent reliably by the transaction layer (they
       are not retransmitted by it) and do not cause a change in the state of
       the server transaction ...
    */
    *is_for_transp = true;

    if (status_code >= 200 && status_code < 300) { // NOLINT
      /*
        According rfc 3261 17.2.1 INVITE Server Transaction:
         If, while in the "Proceeding" state, the TU passes a 2xx response to
         the server transaction, the server transaction MUST pass this response
         to the transport layer for transmission. It is not retransmitted by the
         server transaction; retransmissions of 2xx responses are handled by the
         TU. The server transaction MUST then transition to the "Terminated"
         state ...
      */
      strans.get->state = __SipServerTransactionState_INVITE_TERMINATED;
    }
    if (status_code >= 300 && status_code < 699) { // NOLINT
      /*
        According rfc 3261 17.2.1 INVITE Server Transaction:
         While in the "Proceeding" state, if the TU passes a response with
         status code from 300 to 699 to the server transaction, the response
         MUST be passed to the transport layer for transmission, and the state
         machine MUST enter the "Completed" state. For unreliable transports,
         timer G is set to fire in T1 seconds, and is not set to fire for
         reliable transports ...
      */
      strans.get->state = __SipServerTransactionState_INVITE_COMPLETED;

      if (!SipTransportPtr_is_reliable(strans.get->sip_core.get->sip_transp)) {
        err = TimerFdPtr_create(
            strans.get->sip_core.get->evl, 0, __SIP_CORE_STRANS_T1,
            SipServerTransactionPtr_g_unrliable_timeouth,
            GenericPtr_from_arc(SipServerTranscationPtr, strans),
            &strans.get->invite_3xx_6xx_timer);
        if (err) {
          goto error_out;
        }
      }
      /*
        According rfc 3261 17.2.1 INVITE Server Transaction:
          When the "Completed" state is entered, timer H MUST be set to fire in
          64*T1 seconds for all transports. Timer H determines when the server
          transaction abandons retransmitting the response. ...

          If timer H fires while in the "Completed" state, it implies that the
          ACK was never received. In this case, the server transaction MUST
          transition to the "Terminated" state, and MUST indicate to the TU
          that a transaction failure has occurred.
   */
    }
    break;

  case __SipServerTransactionState_INVITE_COMPLETED:
    /*
      According rfc 3261 17.2.1 INVITE Server Transaction:
       If an ACK is received while the server transaction is in the
       "Completed" state, the server transaction MUST transition to the
       "Confirmed" state. As Timer G is ignored in this state, any
       retransmissions of the response will cease.
     */
    break;

  case __SipServerTransactionState_INVITE_CONFIRMED:
    /*
      According rfc 3261 17.2.1 INVITE Server Transaction:
       The purpose of the "Confirmed" state is to absorb any additional ACK
       messages that arrive, triggered from retransmissions of the final
       response. When this state is entered, timer I is set to fire in T4
       seconds for unreliable transports, and zero seconds for reliable
       transports. Once timer I fires, the server MUST transition to the
       "Terminated" state.
     */
    break;

  case __SipServerTransactionState_INVITE_TERMINATED:
    /*
      According rfc 3261 17.2.1 INVITE Server Transaction:
       Once the transaction is in the "Terminated" state, it MUST be
       destroyed immediately. As with client transactions, this is needed
       to ensure reliability of the 2xx responses to INVITE.
     */
    break;

  default:
    *is_for_transp = false;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t
SipServerTransactionPtr_g_unrliable_timeouth(struct TimerFdPtr timer,
                                             struct GenericPtr data) {
  /*
    According rfc 3261 17.2.1 INVITE Server Transaction:
     If timer G fires, the response is passed to the transport layer once more
     for retransmission, and timer G is set to fire in MIN(2*T1, T2) seconds.
     From then on, when timer G fires, the response is passed to the transport
     again for transmission, and timer G is reset with a value that doubles,
     unless that value exceeds T2, in which case it is reset with the value of
     T2.
  */
  puts(__func__);
  struct SipServerTransactionPtr strans =
      GenericPtr_dump(SipServerTransactionPtr, data);
  struct csview reason_phrase;
  uint32_t status_code;
  cme_error_t err;
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

  err = TimerFdPtr_create(strans.get->sip_core.get->evl, 0,
                          timer.get->seconds * (long)2,
                          SipServerTransactionPtr_g_unrliable_timeouth,
                          GenericPtr_from_arc(SipServerTranscationPtr, strans),
                          &strans.get->invite_3xx_6xx_timer);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}
