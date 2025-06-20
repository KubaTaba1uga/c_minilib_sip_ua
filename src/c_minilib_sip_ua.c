#include <errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "c_minilib_init.h"

#include "event_loop/event_loop.h"
#include "sip_core/sip_core.h"
#include "sip_transp/sip_transp.h"

static void log_func(enum cmi_LogLevelEnum _, char *data) {
  printf("%s", data);
}

static cme_error_t sip_core_request_handler(sip_msg_t sip_msg, ip_t peer_ip,
                                            sip_strans_t strans,
                                            sip_core_t sip_core, void *data) {
  puts("Received Sip msg in MAIN!");
  return 0;
}

int main(void) {
  event_loop_t evl;
  cme_error_t err;

  cmi_configure(log_func);

  if (cme_init()) {
    err = cme_error(EINVAL, "Cannot initialize error library");
    goto error_out;
  }

  err = cmi_init();
  if (err) {
    goto error_out;
  }

  err = event_loop_create(&evl);
  if (err) {
    goto error_out;
  }

  sip_core_t sip_core;
  err = sip_core_create(evl, (ip_t){.ip = "127.0.0.1", .port = "7337"},
                        SupportedSipTranspProtos_UDP, &sip_core);
  if (err) {
    goto error_event_loop_cleanup;
  }

  err = sip_listen(sip_core_request_handler, NULL, sip_core);
  if (err) {
    goto error_sip_core_cleanup;
  }

  log_func(0, "Starting event loop...\n");
  err = event_loop_start(evl);
  if (err) {
    goto error_sip_core_cleanup;
  }

  sip_core_destroy(&sip_core);
  event_loop_destroy(&evl);
  cmi_destroy();

  return 0;

error_sip_core_cleanup:
  sip_core_destroy(&sip_core);
error_event_loop_cleanup:
  event_loop_destroy(&evl);
error_out:
  printf("Error: %s\n", err->msg);
  cme_error_dump_to_file(err, "error.txt");
  return 1;
}
