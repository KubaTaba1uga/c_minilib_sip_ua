#include "sip_transport/sip_transport.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/_internal/sip_transport_listen.h"
#include "sip_transport/_internal/sip_transport_send.h"
#include "utils/memory.h"

cme_error_t SipTransportPtr_create(struct EventLoopPtr evl,
                                   struct IpAddrPtr ip_addr,
                                   enum SipTransportProtocolType proto_type,
                                   struct SipTransportPtr *out) {
  struct __SipTransport *sip_transp = my_malloc(sizeof(struct __SipTransport));

  *sip_transp = (struct __SipTransport){
      .proto_type = proto_type,
      // Init Event Loop
      .evl = EventLoopPtr_clone(evl),
  };

  *out = SipTransportPtr_from_ptr(sip_transp);

  cme_error_t err;
  switch (proto_type) {
  case SipTransportProtocolType_UDP:
    // Init Udp Socket
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
  *out = (struct SipTransportPtr){0};
  return cme_return(err);
};

void __SipTransport_destroy(struct __SipTransport *sip_transp) {
  // Cleanup Udp socket
  UdpSocketPtr_drop(&sip_transp->udp_socket);

  // Cleanup Event loop
  EventLoopPtr_drop(&sip_transp->evl);
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
  return sip_transpp.get->proto_type != SipTransportProtocolType_UDP;
};
