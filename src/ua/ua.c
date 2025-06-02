#include "ua/_internal/ua.h"
#include "ua/ua.h"

cme_error_t ua_create(evl_t evl, ua_t *out) {
  return cmsu_Ua_create(evl, out);
};

void ua_destroy(ua_t *out) { cmsu_Ua_destroy(out); }
