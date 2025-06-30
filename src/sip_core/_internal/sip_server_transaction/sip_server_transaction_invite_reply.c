#include "c_minilib_error.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction_invite.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/sip_transport.h"
#include "timer_fd/_internal/timer_fd.h"
#include "timer_fd/timer_fd.h"

#define __SIP_CORE_STRANS_T1 500000000

static cme_error_t __SipServerTransactionPtr_reply_handler_INVITE_PROCEEDING(
    uint32_t status_code, cstr status_phrase,
    struct SipServerTransactionPtr strans, bool *is_for_transp);

static cme_error_t
__SipServerTransactionPtr_g_unrliable_timeouth(struct TimerFdPtr timer,
                                               struct GenericPtr arg);

cme_error_t
__SipServerTransactionPtr_invite_reply(uint32_t status_code, cstr status_phrase,
                                       struct SipServerTransactionPtr strans) {
  puts(__func__);
  bool is_for_transport = false;
  struct SipMessagePtr sipmsg;
  struct BufferPtr bytes;
  cme_error_t err;

  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
  case __SipServerTransactionState_INVITE_PROCEEDING:
    err = __SipServerTransactionPtr_reply_handler_INVITE_PROCEEDING(
        status_code, status_phrase, strans, &is_for_transport);
    if (err) {
      goto error_out;
    }

    break;
  }

  if (is_for_transport) {
    err = SipMessagePtr_status_from_request(
        strans.get->init_request, status_code, status_phrase, &sipmsg);
    if (err) {
      goto error_out;
    }

    err = SipMessagePtr_generate(sipmsg, &bytes);
    if (err) {
      goto error_sip_msg_cleanup;
    }

    err = SipTransportPtr_send(strans.get->sip_core.get->sip_transp,
                               strans.get->last_peer_ip, bytes);
    if (err) {
      goto error_bytes_cleanup;
    }
  }

  return 0;

error_bytes_cleanup:
  BufferPtr_drop(&bytes);
error_sip_msg_cleanup:
  SipMessagePtr_drop(&sipmsg);
error_out:
  return cme_return(err);
}

static cme_error_t __SipServerTransactionPtr_reply_handler_INVITE_PROCEEDING(
    uint32_t status_code, cstr status_phrase,
    struct SipServerTransactionPtr strans, bool *is_for_transp) {
  cme_error_t err;

  /*
    According rfc 3261 17.2.1 INVITE Server Transaction:
     The TU passes any number of provisional responses to the server
     transaction. So long as the server transaction is in the "Proceeding"
     state, each of these MUST be passed to the transport layer for
     transmission. They are not sent reliably by the transaction layer (they are
     not retransmitted by it) and do not cause a change in the state of the
     server transaction.
  */
  if (status_code >= 100 && status_code <= 199) { // NOLINT
    *is_for_transp = true;
  }

  /*
    According rfc 3261 17.2.1 INVITE Server Transaction:
     If, while in the "Proceeding" state, the TU passes a 2xx response to the
     server transaction, the server transaction MUST pass this response to the
     transport layer for transmission. It is not retransmitted by the server
     transaction; retransmissions of 2xx responses are handled by the TU. The
     server transaction MUST then transition to the "Terminated" state.
  */
  if (status_code >= 200 && status_code <= 299) { // NOLINT
    *is_for_transp = true;
    strans.get->state = __SipServerTransactionState_INVITE_TERMINATED;
    err = __SipServerTransactionPtr_on_INVITE_TERMINATED(strans);
    if (err) {
      goto error_out;
    }
  }

  /*
    According rfc 3261 17.2.1 INVITE Server Transaction:
     While in the "Proceeding" state, if the TU passes a response with
     status code from 300 to 699 to the server transaction, the response
     MUST be passed to the transport layer for transmission, and the state
     machine MUST enter the "Completed" state. For unreliable transports,
     timer G is set to fire in T1 seconds, and is not set to fire for
     reliable transports.
  */
  if (status_code >= 300 && status_code <= 699) { // NOLINT
    *is_for_transp = true;
    strans.get->state = __SipServerTransactionState_INVITE_COMPLETED;
    if (!SipTransportPtr_is_reliable(strans.get->sip_core.get->sip_transp)) {
      err = TimerFdPtr_create(
          strans.get->sip_core.get->evl, 0, __SIP_CORE_STRANS_T1,
          __SipServerTransactionPtr_g_unrliable_timeouth,
          GenericPtr_from_arc(SipServerTranscationPtr, strans),
          &strans.get->invite_3xx_6xx_timer);
      if (err) {
        goto error_out;
      }
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

static cme_error_t
__SipServerTransactionPtr_g_unrliable_timeouth(struct TimerFdPtr timer,
                                               struct GenericPtr arg) {
  puts(__func__);
  return 0;
}
