#include <stdio.h>

#include "c_minilib_error.h"
#include "c_minilib_init.h"
#include "event_loop/event_loop.h"
#include "transp/transp.h"

static int cmsu_sip_init(void) {
  struct cmsu_TransportUdp *udp_transp;
  cme_error_t err;

  err = cmsu_udp_create(&udp_transp);

  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return err->code;
}

static void cmsu_sip_destroy(void) { cmsu_evl_destroy(&evl); }

CMI_REGISTER(sip, cmsu_sip_init, cmsu_sip_destroy,
             (const char *[]){"event_loop"}, 1);
