#include <errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_core/sip_core.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"

cme_error_t __sip_core_request_handler(sip_msg_t sip_msg, ip_t peer_ip,
                                       sip_strans_t strans, sip_core_t sip_core,
                                       void *data) {
  puts("Received sip msg!!! :)");
  return 0;
}

/* static cme_error_t __sip_transp_recvh_t(sip_msg_t sip_msg, ip_t peer_ip, */
/*                                         sip_transp_t sip_transp, void *data)
 * { */
/*   puts("Received sip msg!!! :)"); */
/*   return 0; */
/* } */

/* static cme_error_t __udp_transp_recvh_t(cstr buf, ip_t peer_ip, void *data) {
 */
/*   puts("Received udp msg!!! :)"); */
/*   return 0; */
/* } */

int main(void) {
  cme_error_t err;

  if (cme_init()) {
    err = cme_error(EINVAL, "Cannot initialize error library");
    goto error_out;
  }

  event_loop_t evl;
  err = event_loop_create(&evl);
  if (err) {
    goto error_out;
  }

  sip_core_t sip_core;
  err = sip_core_create(evl, (ip_t){.ip = "0.0.0.0", .port = "7337"},
                        SupportedSipTranspProtos_UDP, &sip_core);
  if (err) {
    goto error_evl_cleanup;
  }

  err = sip_core_listen(__sip_core_request_handler, NULL, sip_core);
  if (err) {
    goto error_core_cleanup;
  }

  puts("Starting event loop...\n");
  err = event_loop_start(evl);
  if (err) {
    goto error_core_cleanup;
  }

  /* udp_socket_deref(udp); */
  sip_core_deref(sip_core);
  event_loop_deref(evl);
  cme_destroy();

  return 0;
error_core_cleanup:
  sip_core_deref(sip_core);
error_evl_cleanup:
  event_loop_deref(evl);
error_out:
  printf("Error: %s\n", err->msg);
  cme_error_dump_to_file(err, "error.txt");
  cme_destroy();
  return 1;
}
