#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_core_strans.h"
#include "utils/sip_msg.h"
#include "utils/sip_status_codes.h"

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_NONE(
    struct SipServerTransactionPtr strans);

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_PROCEEDING(
    struct SipServerTransactionPtr strans);

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_COMPLETED(
    struct SipServerTransactionPtr strans);

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_CONFIRMED(
    struct SipServerTransactionPtr strans);

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_TERMINATED(
    struct SipServerTransactionPtr strans);

/*
   Once sip core receives server related to server invite transaction
   it will always run this function.
*/
cme_error_t __SipServerTransactionPtr_invite_recv_next_state(
    struct SipServerTransactionPtr strans) {
  cme_error_t err;

  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    err = __SipServerTransactionPtr_on_recv_INVITE_NONE(strans);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_PROCEEDING:
    err = __SipServerTransactionPtr_on_recv_INVITE_PROCEEDING(strans);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_COMPLETED:
    err = __SipServerTransactionPtr_on_recv_INVITE_COMPLETED(strans);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_CONFIRMED:
    err = __SipServerTransactionPtr_on_recv_INVITE_CONFIRMED(strans);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_TERMINATED:
    err = __SipServerTransactionPtr_on_recv_INVITE_TERMINATED(strans);
    if (err) {
      goto error_out;
    }
    break;
  }

  return 0;

error_out:
  return cme_return(err);
}

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_NONE(
    struct SipServerTransactionPtr strans) {
  /*
    According rfc 3261 17.2.1 INVITE Server Transaction:
      When a server transaction is constructed for a request, it enters the
      "Proceeding" state. The server transaction MUST generate a 100
      (Trying) response ...
  */
  puts(__func__);
  cme_error_t err;
  err = SipServerTransactionPtr_internal_reply(SIP_STATUS_TRYING,
                                               cstr_from("Trying"), strans);
  if (err) {
    goto error_out;
  }

  strans.get->state = __SipServerTransactionState_INVITE_PROCEEDING;

  return 0;

error_out:
  return cme_return(err);
}

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_PROCEEDING(
    struct SipServerTransactionPtr strans) {
  /*
    According rfc 3261 17.2.1 INVITE Server Transaction:
      If a request retransmission is received while in the "Proceeding" state,
      the most recent provisional response that was received from the TU MUST
      be passed to the transport layer for retransmission ...
  */
  puts(__func__);
  struct csview reason_phrase;
  uint32_t status_code;
  cme_error_t err;

  SipMessagePtr_get_status_code_and_reason(strans.get->last_response,
                                           &status_code, &reason_phrase);

  err = SipServerTransactionPtr_internal_reply(
      status_code, cstr_from_sv(reason_phrase), strans);
  if (err) {
    goto error_out;
  }

  return 0;
error_out:
  return cme_return(err);
}

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_COMPLETED(
    struct SipServerTransactionPtr strans) {
  /* According rfc 3261 17.2.1 INVITE Server Transaction:
       If an ACK is received while the server transaction is in the
       "Completed" state, the server transaction MUST transition to the
       "Confirmed" state. As Timer G is ignored in this state, any
       retransmissions of the response will cease.
  */
  puts(__func__);
  // TO-DO handle COMPLETED state ACK

  strans.get->state = __SipServerTransactionState_INVITE_CONFIRMED;

  return 0;
}

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_CONFIRMED(
    struct SipServerTransactionPtr strans) {
  puts(__func__);
  return 0;
}

static cme_error_t __SipServerTransactionPtr_on_recv_INVITE_TERMINATED(
    struct SipServerTransactionPtr strans) {
  puts(__func__);
  return 0;
}
