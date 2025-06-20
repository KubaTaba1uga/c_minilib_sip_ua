#include "sip_transport/sip_transport.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/_internal/sip_transport_listen.h"
#include "sip_transport/_internal/sip_transport_send.h"

cme_error_t sip_transp_create(event_loop_t evl, ip_t ip_addr,
                              enum SupportedSipTranspProtos proto_type,
                              sip_transp_t *out) {
  return __SipTransport_create(evl, ip_addr, proto_type, out);
}

cme_error_t sip_transp_listen(sip_transp_t sip_transp, sip_transp_recvh_t recvh,
                              void *arg) {
  return __SipTransport_listen(sip_transp, recvh, arg);
};

sip_transp_t sip_transp_ref(sip_transp_t sip_transpp) {
  return __SipTransport_ref(sip_transpp);
};

void sip_transp_deref(sip_transp_t sip_transpp) {
  __SipTransport_deref(sip_transpp);
};

cme_error_t sip_transp_send(sip_transp_t sip_transpp, ip_t ip_addr,
                            csview_ptr_t bytes) {
  return __SipTransport_send(sip_transpp, ip_addr, bytes);
}

bool sip_transp_is_reliable(sip_transp_t sip_transpp) {
  return __SipTransport_is_reliable(sip_transpp);
};
