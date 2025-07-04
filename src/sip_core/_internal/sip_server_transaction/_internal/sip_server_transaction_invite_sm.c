#include <assert.h>

#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_server_transaction/_internal/sip_server_transaction_invite.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "timer_fd/_internal/timer_fd.h"

static inline cme_error_t __SipServerTransactionPtr_on_INVITE_PROCEEDING(
    struct SipServerTransactionPtr strans);
static inline cme_error_t __SipServerTransactionPtr_on_INVITE_COMPLETED(
    struct SipServerTransactionPtr strans);
static inline cme_error_t __SipServerTransactionPtr_on_INVITE_CONFIRMED(
    struct SipServerTransactionPtr strans);
static inline cme_error_t __SipServerTransactionPtr_on_INVITE_TERMINATED(
    struct SipServerTransactionPtr strans);
static inline cme_error_t
__SipServerTransactionPtr_invite_g_timerh(struct TimerFdPtr timer,
                                          struct GenericPtr arg);
static inline cme_error_t
__SipServerTransactionPtr_invite_h_timerh(struct TimerFdPtr timer,
                                          struct GenericPtr arg);
static inline cme_error_t
__SipServerTransactionPtr_invite_i_timerh(struct TimerFdPtr timer,
                                          struct GenericPtr arg);

cme_error_t __SipServerTransactionPtr_invite_move_to_state(
    enum __SipServerTransactionState next_state,
    struct SipServerTransactionPtr strans) {
  cme_error_t err;

  if (next_state == strans.get->__state) {
    return 0;
  }

  switch (next_state) {
  case __SipServerTransactionState_NONE:
    assert(false);

  case __SipServerTransactionState_INVITE_PROCEEDING:
    err = __SipServerTransactionPtr_on_INVITE_PROCEEDING(strans);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_COMPLETED:
    err = __SipServerTransactionPtr_on_INVITE_COMPLETED(strans);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_CONFIRMED:
    err = __SipServerTransactionPtr_on_INVITE_CONFIRMED(strans);
    if (err) {
      goto error_out;
    }
    break;

  case __SipServerTransactionState_INVITE_TERMINATED:
    err = __SipServerTransactionPtr_on_INVITE_TERMINATED(strans);
    if (err) {
      goto error_out;
    }
    break;
  }

  (void)__SipServerTransactionPtr_invite_g_timerh;
  (void)__SipServerTransactionPtr_invite_h_timerh;
  (void)__SipServerTransactionPtr_invite_i_timerh;
  strans.get->__state = next_state;

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t __SipServerTransactionPtr_on_INVITE_PROCEEDING(
    struct SipServerTransactionPtr strans) {
  /*
    According rfc 3261 17.2.1 INVITE Server Transaction:
      When a server transaction is constructed for a request, it enters the
      "Proceeding" state. The server transaction MUST generate a 100
      (Trying) response ...
  */
  puts(__func__);
  cme_error_t err;
  err = __SipServerTransactionPtr_invite_reply(SIP_STATUS_TRYING,
                                               cstr_from("Trying"), strans);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t __SipServerTransactionPtr_on_INVITE_COMPLETED(
    struct SipServerTransactionPtr strans) {
  SipServerTransactionPtr_drop(&strans);
  /* According rfc 3261 17.2.1 INVITE Server Transaction:
      For unreliable transports, timer G is set to fire in T1 seconds, and is
     not set to fire for reliable transports. ...
     When the "Completed" state is entered, timer H MUST be set to fire in 64*T1
     seconds for all transports. Timer H determines when the server transaction
     abandons retransmitting the response. Its value is chosen to equal Timer B,
     the amount of time a client transaction will continue to retry sending a
     request.
  */
  // TO-DO set up timer G
  // TO-DO set up timer H

  return 0;
}

static inline cme_error_t __SipServerTransactionPtr_on_INVITE_CONFIRMED(
    struct SipServerTransactionPtr strans) {
  /* According rfc 3261 17.2.1 INVITE Server Transaction:
      As Timer G is ignored in this state, any retransmissions of the
      response will cease. ...
      The purpose of the "Confirmed" state is to absorb any additional ACK
      messages that arrive, triggered from retransmissions of the final
      response. When this state is entered, timer I is set to fire in T4
      seconds for unreliable transports, and zero seconds for reliable
      transports. Once timer I fires, the server MUST transition to the
      "Terminated" state.
  */
  // TO-DO drop timer G
  // TO-DO set up timer I

  return 0;
}

static inline cme_error_t __SipServerTransactionPtr_on_INVITE_TERMINATED(
    struct SipServerTransactionPtr strans) {
  /* According rfc 3261 17.2.1 INVITE Server Transaction:
      Once the transaction is in the "Terminated" state, it MUST be
      destroyed immediately. As with client transactions, this is needed
      to ensure reliability of the 2xx responses to INVITE.
  */
  // TO-DO drop timer H if exists
  // TO-DO drop timer G if exists
  // TO-DO drop timer I if exists
  SipServerTransactionPtr_drop(&strans);

  return 0;
}

static inline cme_error_t
__SipServerTransactionPtr_invite_g_timerh(struct TimerFdPtr timer,
                                          struct GenericPtr arg) {

  return 0;
};

static inline cme_error_t
__SipServerTransactionPtr_invite_h_timerh(struct TimerFdPtr timer,
                                          struct GenericPtr arg) {
  /* According rfc 3261 17.2.1 INVITE Server Transaction:
      If timer H fires while in the "Completed" state, it implies that the
      ACK was never received. In this case, the server transaction MUST
      transition to the "Terminated" state, and MUST indicate to the TU
      that a transaction failure has occurred.
  */
  // TO-DO implement timer
  return 0;
};

static inline cme_error_t
__SipServerTransactionPtr_invite_i_timerh(struct TimerFdPtr timer,
                                          struct GenericPtr arg) {
  /* According rfc 3261 17.2.1 INVITE Server Transaction:
      When this state is entered, timer I is set to fire in T4
      seconds for unreliable transports, and zero seconds for reliable
      transports. Once timer I fires, the server MUST transition to the
      "Terminated" state.
  */
  return 0;
}
