#include "sip_transp/sip_transp.h"
#include "sip_transp/_internal/sip_transp.h"
#include "sip_transp/_internal/sip_transp_listen.h"

cme_error_t sip_transp_create(event_loop_t evl, ip_t ip_addr,
                              enum SupportedSipTranspProtos proto_type,
                              sip_transp_t *out) {
  return cmsu_SipTransport_create(evl, ip_addr, proto_type, out);
};

cme_error_t sip_transp_listen(sip_transp_t sip_transp, sip_transp_recvh_t recvh,
                              void *arg) {
  return cmsu_SipTransport_listen(sip_transp, recvh, arg);
}
