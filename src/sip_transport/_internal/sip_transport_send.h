#include "event_loop/event_loop.h"
#include "sip_transport/_internal/common.h"
#include "sip_transport/sip_transport.h"
#include <stdio.h>

cme_error_t __SipTransport_send(sip_transp_t sip_transpp, ip_t ip_addr,
                                csview_ptr_t bytes) {
  cme_error_t err;

  switch (sip_transpp->get->proto_type) {
  case SupportedSipTranspProtos_UDP: {
    err = udp_socket_send(sip_transpp->get->udp_socket, ip_addr, bytes);
    if (err) {
      goto error_out;
    }

  } break;

  default:
    err = cme_error(EINVAL, "Unsupported transport protocol");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}
