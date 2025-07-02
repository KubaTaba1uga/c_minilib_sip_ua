#include "c_minilib_error.h"
#include "sip_core/sip_core.h"
#include "sip_session/sip_session.h"
#include "utils/generic_ptr.h"
#include "utils/sip_msg.h"
#include <stdio.h>

static cme_error_t __SipSessionPtr_sip_core_reqh_t(struct SipMessagePtr sip_msg,
                                                   struct IpAddrPtr peer_ip,
                                                   struct SipCorePtr sip_core,
                                                   struct GenericPtr arg);
static void __SipSessionPtr_sip_core_strans_errh(
    cme_error_t err, struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg);

cme_error_t SipSessionPtr_listen(struct SipCorePtr sip_core,
                                 sip_session_next_stateh_t next_stateh,
                                 struct GenericPtr arg) {
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

static cme_error_t __SipSessionPtr_sip_core_reqh_t(struct SipMessagePtr sip_msg,
                                                   struct IpAddrPtr peer_ip,
                                                   struct SipCorePtr sip_core,
                                                   struct GenericPtr arg) {
  puts(__func__);

  cme_error_t err;

  csview sip_method;
  void *result = SipMessagePtr_get_method(sip_msg, &sip_method);
  if (!result || !csview_equals(sip_method, "INVITE")) {
    return 0;
  }

  struct SipServerTransactionPtr strans;
  err = SipServerTransactionPtr_create(sip_msg, sip_core, peer_ip,
                                       __SipSessionPtr_sip_core_strans_errh,
                                       arg, &strans);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static void __SipSessionPtr_sip_core_strans_errh(
    cme_error_t err, struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg) {
  puts(err->msg);
}
