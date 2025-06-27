#include <stdlib.h>

#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_core_listen.h"
#include "sip_core/_internal/sip_core_strans.h"
#include "sip_core/_internal/sip_core_strans_map.h"
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

cme_error_t SipCorePtr_listen(sip_core_connh_t connh,
                              struct GenericPtr connh_arg,
                              struct SipCorePtr sip_core) {
  return __SipCore_listen(connh, connh_arg, sip_core);
};

cme_error_t SipCorePtr_accept(struct SipCoreAcceptOps accept_ops,
                              struct SipServerTransactionPtr sip_strans) {
  return __SipCore_accept(accept_ops, sip_strans);
}

cme_error_t
SipCorePtr_reject_busy_here(struct SipServerTransactionPtr sip_strans) {
  return __SipCore_reject_busy_here(sip_strans);
};
