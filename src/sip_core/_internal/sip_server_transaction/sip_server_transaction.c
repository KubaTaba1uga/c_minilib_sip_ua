#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "sip_core/_internal/sip_server_transaction/_internal/sip_server_transaction_invite.h"

cme_error_t
SipServerTransactionPtr_reply(uint32_t status_code, cstr status_phrase,
                              struct SipServerTransactionPtr *sip_strans) {
  cme_error_t err;

  switch (sip_strans->get->__type) {
  case __SipServerTransactionType_INVITE:
    return __SipServerTransactionPtr_invite_reply(status_code, status_phrase,
                                                  *sip_strans);
  case __SipServerTransactionType_NONINVITE:
    err = cme_error(
        ENOENT, "Non-invite server transactions are currently not supported");
    goto error_out;
  }

error_out:
  return cme_return(err);
}

void SipServerTransactionPtr_set_errh(sip_core_strans_errh_t errh,
                                      struct GenericPtr arg,
                                      struct SipServerTransactionPtr strans) {
  strans.get->__errh = errh;
  strans.get->__errh_arg = arg;
}
