#include <errno.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "c_minilib_init.h"

static void log_func(enum cmi_LogLevelEnum _, char *data) {
  printf("%s", data);
}

int main(void) {
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
  log_func(0, "Starting ua...\n");
  cmi_destroy();

  return 0;
error_out:
  printf("Error: %s\n", err->msg);
  cme_error_dump_to_file(err, "error.txt");
  return 1;
}
