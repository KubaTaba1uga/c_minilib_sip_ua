#ifndef C_MINILIB_SIP_UA_TRANSPORT_H
#define C_MINILIB_SIP_UA_TRANSPORT_H

#include "c_minilib_error.h"
enum cmsu_TransportSupported {
  cmsu_TransportSupported_UDP,
};

struct cmsu_TranspProto {
  cme_error_t (*init)(void *data);
  cme_error_t (*create_fd)(void *data, int *fd);
  cme_error_t (*receive)(const int fd);
  cme_error_t (*send)(const int fd);
};

#endif
