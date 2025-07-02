#include "sip_session/sip_session.h"
#include "c_minilib_error.h"
#include "sip_core/sip_core.h"
#include "utils/generic_ptr.h"
#include <stdio.h>

static cme_error_t __SipSessionPtr_sip_core_reqh_t(
    struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg);

cme_error_t SipSessionPtr_listen(struct SipCorePtr sip_core) {
  cme_error_t err;

  err = SipCorePtr_listen(__SipSessionPtr_sip_core_reqh_t,
                          GenericPtr_from_arc(SipCorePtr, sip_core), sip_core);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static cme_error_t __SipSessionPtr_sip_core_reqh_t(
    struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg) {
  puts(__func__);

  return 0;
}
