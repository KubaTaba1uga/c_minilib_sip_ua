#ifndef C_MINILIB_SIP_UA_ERROR_H
#define C_MINILIB_SIP_UA_ERROR_H

#include "c_minilib_error.h"

#define CMSU_CHECK_ERR(err)                                                    \
  if (err) {                                                                   \
    return cme_return(err);                                                    \
  }

#endif
