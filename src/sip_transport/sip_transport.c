#include "sip_transport/sip_transport.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/_internal/sip_transport_listen.h"
#include "sip_transport/_internal/sip_transport_send.h"
#include "utils/memory.h"

cme_error_t SipTransportPtr_create(struct EventLoopPtr evl,
                                   struct IpAddrPtr ip_addr,
                                   enum SipTransportProtocolType proto_type,
                                   struct SipTransportPtr *out) {
  cme_error_t err;

  ;

  *out = SipTransportPtr_from_ptr(my_calloc(1, sizeof(struct __SipTransport)));
  *out->get = (struct __SipTransport){
      .proto_type = proto_type,
      .evl = EventLoopPtr_clone(evl),
  };

  switch (proto_type) {
  case SipTransportProtocolType_UDP:
    err = UdpSocketPtr_create(evl, ip_addr, &out->get->udp_socket);
    if (err) {
      goto error_out;
    }
    break;
  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_out;
  }

  return 0;

error_out:
  SipTransportPtr_drop(out);
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
                                   sip_transp_recvh_t recvh,
                                   struct GenericPtr arg) {
  return __SipTransport_listen(sip_transpp, recvh, arg);
};

cme_error_t SipTransportPtr_send(struct SipTransportPtr *sip_transpp,
                                 struct IpAddrPtr ip_addr,
                                 struct BufferPtr bytes) {
  return __SipTransport_send(sip_transpp, ip_addr, bytes);
};

bool SipTransportPtr_is_reliable(struct SipTransportPtr sip_transpp) {
  return sip_transpp.get->proto_type == SipTransportProtocolType_UDP;
};
