#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"

static cme_error_t __SipServerTransactionPtr_on_INVITE_TERMINATED(
    struct SipServerTransactionPtr strans);
static cme_error_t __SipServerTransactionPtr_on_INVITE_COMPLETED(
    struct SipServerTransactionPtr strans);

/*
   Ivite server transaction is created on incoming INVITE sip request.
 It is created with NONE state. When transaction try to reply 100
 trying to invite the transaction state move to PROCEEDING. From now
 on the TU is responsible for moving transaction styate further.
 So SM next_state func can be triggered from recive or from user.
 That's why we need some state hanlders in this sm file, this makes
 them accesible by recv and by send.

 */
cme_error_t __SipServerTransactionPtr_move_to_state(
    enum __SipServerTransactionState next_state,
    struct SipServerTransactionPtr strans);
static cme_error_t __SipServerTransactionPtr_on_INVITE_TERMINATED(
    struct SipServerTransactionPtr strans) {
  SipServerTransactionPtr_drop(&strans);

  return 0;
}

static cme_error_t __SipServerTransactionPtr_on_INVITE_COMPLETED(
    struct SipServerTransactionPtr strans) {
  SipServerTransactionPtr_drop(&strans);

  return 0;
}
