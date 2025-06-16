#include "sip_transport/sip_transport.h"
#include "sip_transport/_internal/sip_transport.h"

cme_error_t sip_transp_create(event_loop_t evl, ip_t ip_addr,
                              enum SupportedSipTranspProtos sstp,
                              sip_transp_t *out) {
  return __SipTransport_create(evl, ip_addr, sstp, out);
}
