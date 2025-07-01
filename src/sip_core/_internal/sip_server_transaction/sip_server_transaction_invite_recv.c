#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction_invite.h"
#include "utils/sip_msg.h"
#include "utils/sip_status_codes.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static cme_error_t __SipServerTransactionPtr_recv_handler_INVITE_PROCEEDING(
    struct SipMessagePtr sipmsg, struct SipServerTransactionPtr strans,
    bool *is_for_tu);

/*
   Once sip core receives request related to server invite transaction
   it will always run this function.
*/
cme_error_t
__SipServerTransactionPtr_invite_recv(struct SipMessagePtr sipmsg,
                                      struct SipServerTransactionPtr strans) {
  cme_error_t err;

  bool is_for_tu = false;

  switch (strans.get->state) {
  case __SipServerTransactionState_NONE:
    err = __SipServerTransactionPtr_move_to_state(
        __SipServerTransactionState_INVITE_PROCEEDING, strans);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_PROCEEDING:
    err = __SipServerTransactionPtr_recv_handler_INVITE_PROCEEDING(
        sipmsg, strans, &is_for_tu);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_COMPLETED: {
    /*
      According rfc 3261 17.2.1 INVITE Server Transaction:
       If an ACK is received while the server transaction is in the
      "Completed" state, the server transaction MUST transition to the
      "Confirmed" state.
    */
    struct cmsc_String method = cmsc_bs_msg_to_string(
        &sipmsg.get->sip_msg->request_line.sip_method, sipmsg.get->sip_msg);
    if (strncmp(method.buf, "ACK", method.len) == 0) {
      err = __SipServerTransactionPtr_move_to_state(
          __SipServerTransactionState_INVITE_CONFIRMED, strans);
      if (err) {
        goto error_out;
      }
    }
  } break;

  case __SipServerTransactionState_INVITE_CONFIRMED:
  case __SipServerTransactionState_INVITE_TERMINATED:
    assert(false);
  }

  if (is_for_tu) {
    err = strans.get->tu_ops.reqh(sipmsg, strans.get->sip_core, strans,
                                  strans.get->tu_ops.arg);
    if (err) {
      printf("Error in server transaction tu.reqh: %s\n", err->msg);
    }
  }

  return 0;

error_out:
  strans.get->tu_ops.errh(err, sipmsg, strans.get->sip_core, strans,
                          strans.get->tu_ops.arg);
  return cme_return(err);
}

static cme_error_t __SipServerTransactionPtr_recv_handler_INVITE_PROCEEDING(
    struct SipMessagePtr sipmsg, struct SipServerTransactionPtr strans,
    bool *is_for_tu) {
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

  err = __SipServerTransactionPtr_invite_reply(
      status_code, cstr_from_sv(reason_phrase), strans);
  if (err) {
    goto error_out;
  }

  *is_for_tu = false;

  return 0;

error_out:
  return cme_return(err);
}
