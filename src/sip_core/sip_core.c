#include "sip_core/_internal/sip_core.h"
#include "sip_core/sip_core.h"

cme_error_t sip_core_create(event_loop_t evl, ip_t ip_addr,
                            enum SupportedSipTranspProtos proto_type,
                            sip_core_t *out) {
  return __SipCore_create(evl, ip_addr, proto_type, out);
};

sip_core_t sip_core_ref(sip_core_t sip_corep) {
  return __SipCore_ref(sip_corep);
};

void sip_core_deref(sip_core_t sip_corep) { __SipCore_deref(sip_corep); };

cme_error_t sip_core_listen(sip_core_request_handler_t requesth, void *data,
                            sip_core_t sip_core) {
  return __SipCore_listen(requesth, data, sip_core);
}
