#include <errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_core/sip_core.h"
#include "sip_session/sip_session.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/sip_transport.h"
#include "stc/cstr.h"
#include "udp_socket/udp_socket.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"

int main(void) {
  cme_error_t err;

  if (cme_init()) {
    err = cme_error(EINVAL, "Cannot initialize error library");
    goto error_out;
  }

  struct EventLoopPtr evl;
  err = EventLoopPtr_create(&evl);
  if (err) {
    goto error_out;
  }

  struct SipCorePtr sip_core;
  struct IpAddrPtr ip_addr =
      IpAddrPtr_create(cstr_lit("0.0.0.0"), cstr_lit("7337"));
  err =
      SipCorePtr_create(evl, ip_addr, SipTransportProtocolType_UDP, &sip_core);
  if (err) {
    goto error_evl_cleanup;
  }

  err = SipSessionPtr_listen(sip_core);
  if (err) {
    goto error_core_cleanup;
  }

  puts("Starting event loop...\n");
  err = EventLoopPtr_start(evl);
  if (err) {
    goto error_core_cleanup;
  }

  puts("Event loop done\n");
  printf("ip addr=%s, use_count=%td\n", cstr_data(&ip_addr.get->ip),
         *ip_addr.use_count);

  IpAddrPtr_drop(&ip_addr);
  SipCorePtr_drop(&sip_core);
  EventLoopPtr_drop(&evl);
  cme_destroy();

  return 0;

error_core_cleanup:
  SipCorePtr_drop(&sip_core);
error_evl_cleanup:
  IpAddrPtr_drop(&ip_addr);
  EventLoopPtr_drop(&evl);
error_out:
  printf("Error: %s\n", err->msg);
  cme_error_dump_to_file(err, "error.txt");
  cme_destroy();
  return 1;
}
