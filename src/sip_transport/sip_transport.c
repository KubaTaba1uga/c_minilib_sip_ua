#include "sip_transport/sip_transport.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/_internal/sip_transport_listen.h"
#include "sip_transport/_internal/sip_transport_send.h"

cme_error_t SipTransportPtr_create(struct EventLoopPtr evl, ip_t ip_addr,
                                   enum SipTransportProtocolType proto_type,
                                   struct SipTransportPtr *out) {
  struct __SipTransport sip_transp = {0};
  cme_error_t err;

  switch (proto_type) {
  case SipTransportProtocolType_UDP:
    err = UdpSocketPtr_create(evl, ip_addr, &sip_transp.udp_socket);
    if (err) {
      goto error_out;
    }
    break;
  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_out;
  }

  sip_transp.proto_type = proto_type;
  sip_transp.evl = EventLoopPtr_clone(evl);

  *out = SipTransportPtr_from(sip_transp);

  return 0;

error_out:
  return cme_return(err);
};

void __SipTransport_destroy(void *data) {
  struct __SipTransport *sip_transp = data;
  UdpSocketPtr_drop(&sip_transp->udp_socket);
  EventLoopPtr_drop(&sip_transp->evl);
};

struct __SipTransport __SipTransport_clone(struct __SipTransport udp_socket) {
  return udp_socket;
};

cme_error_t SipTransportPtr_listen(struct SipTransportPtr *sip_transpp,
                                   sip_transp_recvh_t recvh, void *arg) {
  return __SipTransport_listen(sip_transpp, recvh, arg);
};

cme_error_t SipTransportPtr_send(struct SipTransportPtr *sip_transpp,
                                 ip_t ip_addr, csview_ptr_t bytes) {
  return __SipTransport_send(sip_transpp, ip_addr, bytes);
};

bool SipTransportPtr_is_reliable(struct SipTransportPtr sip_transpp) {
  return sip_transpp.get->proto_type == SipTransportProtocolType_UDP;
};
