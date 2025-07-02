#include <stdlib.h>

#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_core_listen.h"
#include "sip_core/_internal/sip_core_strans_map.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "sip_core/sip_core.h"
#include "sip_transport/sip_transport.h"
#include "utils/memory.h"

cme_error_t SipCorePtr_create(struct EventLoopPtr evl, struct IpAddrPtr ip_addr,
                              enum SipTransportProtocolType proto_type,
                              struct SipCorePtr *out) {
  struct __SipCore *sip_core = my_calloc(1, sizeof(struct __SipCore));
  cme_error_t err;

  err = SipTransportPtr_create(evl, ip_addr, proto_type, &sip_core->sip_transp);
  if (err) {
    goto error_out;
  }

  sip_core->evl = EventLoopPtr_clone(evl);
  sip_core->listeners = my_malloc(sizeof(struct queue__SipCoreListeners));
  *sip_core->listeners = queue__SipCoreListeners_init();
  sip_core->stranses = my_malloc(sizeof(struct hmap__SipServerTransactions));
  *sip_core->stranses = hmap__SipServerTransactions_init();

  *out = SipCorePtr_from_ptr(sip_core);

  return 0;

error_out:
  *out = (struct SipCorePtr){0};
  return cme_return(err);
};

void __SipCore_destroy(struct __SipCore *sip_core) {
  puts(__func__);

  hmap__SipServerTransactions_drop(sip_core->stranses);
  free(sip_core->stranses);
  queue__SipCoreListeners_drop(sip_core->listeners);
  free(sip_core->listeners);

  EventLoopPtr_drop(&sip_core->evl);
  SipTransportPtr_drop(&sip_core->sip_transp);
};

cme_error_t SipCorePtr_listen(sip_core_reqh_t reqh, struct GenericPtr arg,
                              struct SipCorePtr sip_core) {
  return __SipCore_listen(reqh, arg, sip_core);
};

cme_error_t SipServerTransactionPtr_create(
    struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct IpAddrPtr peer_ip, sip_core_strans_errh_t errh,
    struct GenericPtr errh_arg, struct SipServerTransactionPtr *out) {
  return __SipServerTransactionPtr_create(sip_msg, sip_core, peer_ip, errh,
                                          errh_arg, out);
}

cme_error_t
SipServerTransactionPtr_reply(uint32_t status_code, cstr status_phrase,
                              struct SipServerTransactionPtr *sip_strans) {
  return __SipServerTransactionPtr_reply(status_code, status_phrase,
                                         sip_strans);
}
