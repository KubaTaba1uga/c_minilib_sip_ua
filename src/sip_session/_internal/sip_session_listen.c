#include "sip_session/_internal/sip_session_listen.h"
#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/sip_core.h"
#include "sip_session/_internal/sip_session.h"
#include "sip_session/sip_session.h"
#include "utils/generic_ptr.h"
#include "utils/sip_msg.h"
#include <stdio.h>

static cme_error_t __SipSessionPtr_sip_core_reqh_t(
    struct SipMessagePtr sip_msg, struct IpAddrPtr peer_ip,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg);

static inline cme_error_t __SipSessionPtr_next_stateh(
    enum SipSessionState next_state, struct SipMessagePtr sip_msg,
    struct SipCorePtr sip_session, struct GenericPtr arg);

cme_error_t SipSessionPtr_listen(struct SipCorePtr sip_core,
                                 sip_session_next_stateh_t next_stateh,
                                 struct GenericPtr arg) {
  cme_error_t err;

  struct __SipSessionListenerPtr listener;
  __SipSessionListener_create(
      (struct __SipSessionListener){.arg = arg, .next_stateh = next_stateh},
      &listener);

  err = SipCorePtr_listen(__SipSessionPtr_sip_core_reqh_t,
                          GenericPtr_from_arc(__SipSessionListener, listener),
                          sip_core);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static cme_error_t __SipSessionPtr_sip_core_reqh_t(
    struct SipMessagePtr sip_msg, struct IpAddrPtr peer_ip,
    struct SipServerTransactionPtr sip_strans, struct GenericPtr arg) {
  puts(__func__);

  struct __SipSessionListenerPtr session_listener =
      GenericPtr_dump(__SipSessionListenerPtr, arg);
  csview sip_method;
  cme_error_t err;

  void *result = SipMessagePtr_get_method(sip_msg, &sip_method);
  if (!result) {
    return 0;
  }

  if (csview_equals(sip_method, "INVITE")) {
    struct SipSessionPtr sip_session;
    err = __SipSessionPtr_create(sip_strans.get->sip_core, sip_strans,
                                 session_listener.get->next_stateh,
                                 session_listener.get->arg, &sip_session);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t __SipSessionPtr_next_stateh(
    enum SipSessionState next_state, struct SipMessagePtr sip_msg,
    struct SipCorePtr sip_session, struct GenericPtr arg) {
  puts(__func__);

  return 0;
}
