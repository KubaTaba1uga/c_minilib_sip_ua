#include <errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_core/sip_core.h"
#include "sip_transport/_internal/sip_transport.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"
#include "utils/generic_ptr.h"

cme_error_t __sip_core_request_handler(sip_msg_t sip_msg, ip_t peer_ip,
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
  err = SipCorePtr_create(evl, (ip_t){.ip = "0.0.0.0", .port = "7337"},
                          SipTransportProtocolType_UDP, &sip_core);
  if (err) {
    goto error_evl_cleanup;
  }

  err = SipCorePtr_listen(__sip_core_request_handler,
                          GenericPtr_from(SipCorePtr, &sip_core), sip_core);
  if (err) {
    goto error_core_cleanup;
  }

  puts("Starting event loop...\n");
  err = EventLoopPtr_start(evl);
  if (err) {
    goto error_core_cleanup;
  }

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
