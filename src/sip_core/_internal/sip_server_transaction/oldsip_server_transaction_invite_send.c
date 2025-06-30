#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_core_strans.h"
#include "utils/sip_status_codes.h"
#include <assert.h>

static cme_error_t __SipServerTransactionPtr_on_send_INVITE_PROCEEDING(
    uint32_t status_code, struct SipServerTransactionPtr strans,
    bool *is_for_transp);

static cme_error_t __SipServerTransactionPtr_on_send_INVITE_COMPLETED(
    uint32_t status_code, struct SipServerTransactionPtr strans,
    bool *is_for_transp);

static cme_error_t __SipServerTransactionPtr_on_send_INVITE_CONFIRMED(
    uint32_t status_code, struct SipServerTransactionPtr strans,
    bool *is_for_transp);

static cme_error_t __SipServerTransactionPtr_on_send_INVITE_TERMINATED(
    uint32_t status_code, struct SipServerTransactionPtr strans,
    bool *is_for_transp);

cme_error_t __SipServerTransactionPtr_invite_send_next_state(
    uint32_t status_code, struct SipServerTransactionPtr strans,
    bool *is_for_transp) {
  cme_error_t err;

  *is_for_transp = true;

  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    break;
  case __SipServerTransactionState_INVITE_PROCEEDING:
    err = __SipServerTransactionPtr_on_send_INVITE_PROCEEDING(
        status_code, strans, is_for_transp);
    if (err) {
      goto error_out;
    }
  case __SipServerTransactionState_INVITE_COMPLETED:
    err = __SipServerTransactionPtr_on_send_INVITE_COMPLETED(
        status_code, strans, is_for_transp);
    if (err) {
      goto error_out;
    }
  case __SipServerTransactionState_INVITE_CONFIRMED:
    err = __SipServerTransactionPtr_on_send_INVITE_CONFIRMED(
        status_code, strans, is_for_transp);
    if (err) {
      goto error_out;
    }
  case __SipServerTransactionState_INVITE_TERMINATED:
    err = __SipServerTransactionPtr_on_send_INVITE_TERMINATED(
        status_code, strans, is_for_transp);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

static cme_error_t __SipServerTransactionPtr_on_send_INVITE_PROCEEDING(
    uint32_t status_code, struct SipServerTransactionPtr strans,
    bool *is_for_transp) {
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

    return 0;
  }

  static cme_error_t __SipServerTransactionPtr_on_send_INVITE_COMPLETED(
      uint32_t status_code, struct SipServerTransactionPtr strans,
      bool *is_for_transp) {
    return 0;
  }

  static cme_error_t __SipServerTransactionPtr_on_send_INVITE_CONFIRMED(
      uint32_t status_code, struct SipServerTransactionPtr strans,
      bool *is_for_transp) {
    return 0;
  }

  static cme_error_t __SipServerTransactionPtr_on_send_INVITE_TERMINATED(
      uint32_t status_code, struct SipServerTransactionPtr strans,
      bool *is_for_transp) {
    return 0;
  }
