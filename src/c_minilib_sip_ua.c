#include <errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_transport/sip_transport.h"
#include "udp_socket/udp_socket.h"

static cme_error_t __sip_transp_recvh_t(sip_msg_t sip_msg, ip_t peer_ip,
                                        sip_transp_t sip_transp, void *data) {
  puts("Received sip msg!!! :)");
  return 0;
}

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

  /* udp_socket_t udp; */
  /* err = udp_socket_create(evl, (ip_t){.ip = "127.0.0.1", .port = "7337"},
   * &udp); */
  /* if (err) { */
  /*   goto error_out; */
  /* } */

  /* err = udp_socket_listen(udp, __udp_transp_recvh_t, NULL); */
  /* if (err) { */
  /*   goto error_out; */
  /* } */

  sip_transp_t sip_transp;
  err = sip_transp_create(evl, (ip_t){.ip = "0.0.0.0", .port = "7337"},
                          SupportedSipTranspProtos_UDP, &sip_transp);
  if (err) {
    goto error_out;
  }

  err = sip_transp_listen(sip_transp, __sip_transp_recvh_t, NULL);
  if (err) {
    goto error_out;
  }

  puts("Starting event loop...\n");
  err = event_loop_start(evl);
  if (err) {
    goto error_out;
  }

  /* udp_socket_deref(udp); */
  sip_transp_deref(sip_transp);
  event_loop_deref(evl);
  cme_destroy();

  return 0;

error_out:
  printf("Error: %s\n", err->msg);
  cme_error_dump_to_file(err, "error.txt");
  return 1;
}
