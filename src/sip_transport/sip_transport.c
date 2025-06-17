#include "sip_transport/sip_transport.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/_internal/sip_transport_listen.h"

cme_error_t sip_transp_create(event_loop_ptr_t evl, ip_t ip_addr,
                              enum SupportedSipTranspProtos sstp,
                              sip_transp_ptr_t *out) {
  return __SipTransport_create(evl, ip_addr, sstp, out);
}

sip_transp_ptr_t sip_transp_ref(sip_transp_ptr_t sip_transp_ptr) {
  return __SipTransport_ref(sip_transp_ptr);
};

void sip_transp_deref(sip_transp_ptr_t *sip_transp_ptr) {
  __SipTransport_deref(sip_transp_ptr);
};

cme_error_t sip_transp_listen(sip_transp_ptr_t *sip_transp_ptr,
                              sip_transp_recvh_t recvh, void *arg) {
  return __SipTransport_listen(sip_transp_ptr, recvh, arg);
};
