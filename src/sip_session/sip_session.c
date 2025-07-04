#include "sip_session/sip_session.h"
#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/sip_core.h"
#include "stc/cstr.h"
#include "utils/generic_ptr.h"
#include "utils/sip_msg.h"
#include "utils/sip_status_codes.h"
#include <stdio.h>

cme_error_t SipSessionPtr_accept(struct SipSessionPtr *sip_session) {
  cme_error_t err;

  err = SipServerTransactionPtr_reply(SIP_STATUS_OK, cstr_lit("OK"),
                                      &sip_session->get->current_strans);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t SipSessionPtr_ring(struct SipSessionPtr *sip_session) {
  cme_error_t err;

  err = SipServerTransactionPtr_reply(SIP_STATUS_RINGING, cstr_lit("RINGING"),
                                      &sip_session->get->current_strans);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t __SipSessionPtr_create(struct SipCorePtr sip_core,
                                   struct SipServerTransactionPtr strans,
                                   sip_session_next_stateh_t next_stateh,
                                   struct GenericPtr arg,
                                   struct SipSessionPtr *out) {
  struct __SipSession *sip_session = my_malloc(sizeof(struct __SipSession));

  *sip_session = (struct __SipSession){
      .sip_core = SipCorePtr_clone(sip_core),
      .current_strans = SipServerTransactionPtr_clone(strans),
      .next_stateh = next_stateh,
      .arg = arg,
      .state = SipSessionState_INVITED};

  *out = SipSessionPtr_from_ptr(sip_session);

  return 0;
};

void __SipSession_destroy(struct __SipSession *sip_core) {
  SipCorePtr_drop(&sip_core->sip_core);
  SipServerTransactionPtr_drop(&sip_core->current_strans);
};
