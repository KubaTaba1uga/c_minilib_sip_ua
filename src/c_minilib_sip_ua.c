#include <errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_core/sip_core.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"

cme_error_t __sip_core_request_handler(struct SipMessagePtr sip_msg,
                                       struct IpAddrPtr peer_ip,
                                       struct SipCorePtr *sip_core,
                                       struct SipServerTransactionPtr *strans,
                                       struct GenericPtr data) {
  puts("Received sip msg!!! :)");
  return 0;
}

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
  struct IpAddrPtr ip = IpAddrPtr_create(cstr_lit("0.0.0.0"), cstr_lit("7337"));
  err = SipCorePtr_create(evl, ip, SipTransportProtocolType_UDP, &sip_core);
  if (err) {
    goto error_evl_cleanup;
  }

  err = SipCorePtr_listen(__sip_core_request_handler,
                          GenericPtr_from_arc(SipCorePtr, &sip_core), sip_core);
  if (err) {
    goto error_core_cleanup;
  }

  puts("Starting event loop...\n");
  EventLoopPtr_start(evl);
  puts("Event loop done\n");

  IpAddrPtr_drop(&ip);
  SipCorePtr_drop(&sip_core);
  EventLoopPtr_drop(&evl);
  cme_destroy();

  return 0;

error_core_cleanup:
  SipCorePtr_drop(&sip_core);
error_evl_cleanup:
  EventLoopPtr_drop(&evl);
error_out:
  printf("Error: %s\n", err->msg);
  cme_error_dump_to_file(err, "error.txt");
  cme_destroy();
  return 1;
}
