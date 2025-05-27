#include "c_minilib_config.h"
#include "c_minilib_error.h"
#include "c_minilib_init.h"
#include "event_loop/event_loop.h"
#include <stdio.h>

static void log(enum cmi_LogLevelEnum _, char *msg) { printf("%s", msg); }

int main(void) {
  cme_error_t err;

  // This is error lib, we want it available for all modules.
  if (cme_init()) {
    return 1;
  };

  // This is config lib, we want it available for all modules.
  err = cmc_lib_init();
  if (err) {
    puts(err->msg);
    return err->code;
  }

  cmi_configure(log);

  // CMI initiates all app modules.
  err = cmi_init();
  if (err) {
    puts(err->msg);
    return err->code;
  }

  // All initial timers and sockets are created during init step. So at this
  // moment we are ready to trigger the event loop as is.
  /* err = cmsu_event_loop_start(); */
  /* if (err) { */
  /*   puts(err->msg); */
  /*   return err->code; */
  /* } */

  cmi_destroy();

  return 0;
}
