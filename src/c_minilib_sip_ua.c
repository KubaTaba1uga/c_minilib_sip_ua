#include "c_minilib_error.h"
#include "c_minilib_init.h"
#include "event_loop/event_loop.h"
#include "sip/sip.h"
#include "ua/ua.h"
#include <asm-generic/errno-base.h>
#include <stdio.h>

void log_func(enum cmi_LogLevelEnum _, char *data) { printf("%s", data); }

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

  log_func(0, "Creating ua...\n");
  ua_t ua;
  err = ua_create(evl, &ua);
  if (err) {
    goto error_out;
  }

  log_func(0, "Starting event loop...\n");

  err = event_loop_start(evl);
  if (err) {
    goto error_out;
  }

  ua_destroy(&ua);
  event_loop_destroy(&evl);
  cmi_destroy();

  return 0;

error_out:
  puts(err->msg);
  event_loop_destroy(&evl);
  cme_error_dump_to_file(err, "error.txt");
  return 1;
}
