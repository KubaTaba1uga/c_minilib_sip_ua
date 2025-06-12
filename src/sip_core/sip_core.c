#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_core_listen.h"

cme_error_t sip_core_create(event_loop_t evl, ip_t ip_addr,
                            enum SupportedSipTranspProtos sstp,
                            sip_core_t *out) {
  return cmsu_SipCore_create(evl, ip_addr, sstp, out);
}

void sip_core_destroy(sip_core_t *out) { cmsu_SipCore_destroy(out); }

cme_error_t sip_listen(sip_core_request_handler_t requesth, void *data,
                       sip_core_t sip_core) {
  return cmsu_SipCore_listen(requesth, data, sip_core);
}
