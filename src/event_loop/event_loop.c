#include <stdio.h>

#include "c_minilib_error.h"
#include "c_minilib_init.h"
#include "event_loop/event_loop.h"
#include "event_loop/event_loop_internal.h"

static struct cmsu_EventLoop evl;

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

static void cmsu_event_loop_close(void) { ; }

CMI_REGISTER(event_loop, cmsu_event_loop_init, cmsu_event_loop_close, NULL, 0);
