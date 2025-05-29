#include <stdio.h>

#include "c_minilib_error.h"
#include "c_minilib_init.h"
#include "event_loop/event_loop.h"
#include "socket/socket.h"

static int cmsu_sip_init(void) {
  cme_error_t err;

  err = cmsu_event_loop_insert_udp_socket("127.0.0.1", 7337);

  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return err->code;
}

static void cmsu_sip_destroy(void) {}

CMI_REGISTER(sip, cmsu_sip_init, cmsu_sip_destroy,
             (const char *[]){"event_loop"}, 1);
