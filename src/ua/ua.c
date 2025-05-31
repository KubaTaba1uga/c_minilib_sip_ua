#include "ua/_internal/ua.h"
cme_error_t ua_create(cmsu_evl_t evl, ua_t *out) {
  return cmsu_Ua_create(evl, out);
};
