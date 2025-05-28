#include <stdio.h>

#include "c_minilib_error.h"
#include "c_minilib_init.h"
#include "event_loop/event_loop.h"
#include "event_loop/event_loop_internal.h"
#include "event_loop/poll/pollfds.h"

static struct cmsu_EventLoop evl;

cme_error_t cmsu_event_loop_start(void) {
  cme_error_t err;
  err = cmsu_evl_start(&evl);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t
cmsu_event_loop_insert_udp_transp(short revents,
                                  struct cmsu_TransportUdp *transp) {
  return cmsu_evl_insert_udp_transp(revents, transp, &evl);
}

static int cmsu_event_loop_init(void) {
  cme_error_t err;
  err = cmsu_evl_init(&evl);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return err->code;
}

static void cmsu_event_loop_destroy(void) { cmsu_evl_destroy(&evl); }

CMI_REGISTER(event_loop, cmsu_event_loop_init, cmsu_event_loop_destroy, NULL,
             0);
