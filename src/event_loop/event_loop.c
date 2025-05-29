#include <stdio.h>

#include "c_minilib_error.h"
#include "c_minilib_init.h"
#include "event_loop/_internal/event_loop.h"
#include "event_loop/_internal/poll_fd.h"
#include "socket/socket.h"

static struct cmsu_EventLoop evl;

cme_error_t cmsu_event_loop_start(void) {
  cme_error_t err;
  err = cmsu_EventLoop_start(&evl);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t cmsu_event_loop_insert_udp_socket(const char *ipaddr,
                                              uint32_t port) {
  return cmsu_EventLoop_insert_udp_socket(ipaddr, port, &evl);
}

static int cmsu_event_loop_init(void) {
  cme_error_t err;
  err = cmsu_EventLoop_init(&evl);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return err->code;
}

static void cmsu_event_loop_destroy(void) { cmsu_EventLoop_destroy(&evl); }

CMI_REGISTER(event_loop, cmsu_event_loop_init, cmsu_event_loop_destroy, NULL,
             0);
