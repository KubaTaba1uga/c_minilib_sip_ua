#include "c_minilib_error.h"
#include "event_loop/event_loop.h"

int main(void) {
  event_loop_t evl;
  cme_error_t err;

  err = event_loop_create(&evl);
  if (err) {
    goto error_out;
  }

  event_loop_destroy(&evl);

  return 0;

error_out:
  puts(err->msg);
  event_loop_destroy(&evl);
  cme_error_dump_to_file(err, "error.txt");
  return 1;
}
