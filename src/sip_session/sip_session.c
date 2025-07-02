#include "sip_session/sip_session.h"
#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/sip_core.h"
#include "stc/cstr.h"
#include "utils/generic_ptr.h"
#include "utils/sip_msg.h"
#include "utils/sip_status_codes.h"
#include <stdio.h>
static void __SipSessionPtr_sip_core_strans_errh(
    cme_error_t err, struct SipMessagePtr sip_msg,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg);

cme_error_t SipSessionPtr_accept(struct SipSessionPtr *sip_session) {
  cme_error_t err;

  err = SipServerTransactionPtr_reply(SIP_STATUS_ACCEPTED, cstr_lit("OK"),
                                      &sip_session->get->current_strans);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t __SipSessionPtr_create(struct SipCorePtr sip_core,
                                   struct SipServerTransactionPtr sip_strans,
                                   sip_session_next_stateh_t next_stateh,
                                   struct GenericPtr arg,
                                   struct SipSessionPtr *out) {
  struct __SipSession *sip_session = my_malloc(sizeof(struct __SipSession));
  cme_error_t err;

  *sip_session = (struct __SipSession){
      .sip_core = SipCorePtr_clone(sip_core),
      .current_strans = SipServerTransactionPtr_clone(sip_strans),
      .next_stateh = next_stateh,
      .arg = arg,
      .state = SipSessionState_INVITED};

  *out = SipSessionPtr_from_ptr(sip_session);

  sip_strans.get->errh = __SipSessionPtr_sip_core_strans_errh;
  sip_strans.get->errh_arg = GenericPtr_from_arc(SipSessionPtr, *out);

  err =
      out->get->next_stateh(SipSessionState_INVITED,
                            sip_strans.get->init_request, *out, out->get->arg);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  SipSessionPtr_drop(out);
  return cme_return(err);
};

void __SipSession_destroy(struct __SipSession *sip_core) {
  SipCorePtr_drop(&sip_core->sip_core);
  SipServerTransactionPtr_drop(&sip_core->current_strans);
};

static void __SipSessionPtr_sip_core_strans_errh(
    cme_error_t err, struct SipMessagePtr sip_msg,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg) {
  puts(__func__);
  puts(err->msg);
}
