#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_core_listen.h"
#include "sip_core/sip_core.h"
#include "sip_transport/sip_transport.h"

cme_error_t SipCorePtr_create(struct EventLoopPtr evl, ip_t ip_addr,
                              enum SipTransportProtocolType proto_type,
                              struct SipCorePtr *out) {
  struct __SipCore sip_core = {0};
  cme_error_t err;

  err = SipTransportPtr_create(evl, ip_addr, proto_type, &sip_core.sip_transp);
  if (err) {
    goto error_out;
  }

  sip_core.evl = EventLoopPtr_clone(evl);
  sip_core.listeners = queue__SipCoreListenersQueue_init();
  sip_core.stranses = hmap__SipServerTransactionsHMap_init();

  *out = SipCorePtr_from(sip_core);

  return 0;

error_out:
  return cme_return(err);
};

void __SipCore_destroy(void *data) {
  struct __SipCore *sip_core = data;

  hmap__SipServerTransactionsHMap_drop(&sip_core->stranses);
  queue__SipCoreListenersQueue_drop(&sip_core->listeners);
  EventLoopPtr_drop(&sip_core->evl);
  SipTransportPtr_drop(&sip_core->sip_transp);
};

struct __SipCore __SipCore_clone(struct __SipCore sip_core) {
  return sip_core;
};

cme_error_t SipCorePtr_listen(sip_core_request_handler_t requesth, void *data,
                              struct SipCorePtr sip_core){

};
