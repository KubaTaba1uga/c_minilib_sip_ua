#ifndef C_MINILIB_SIP_UA_TRANSPORT_H
#define C_MINILIB_SIP_UA_TRANSPORT_H

#include "c_minilib_error.h"

typedef struct {
  int sockfd;
} cmsu_UdpCtx;

cme_error_t cmsu_udp_create(cmsu_UdpCtx *ctx);

cme_error_t cmsu_udp_send(cmsu_UdpCtx *ctx, const void *b, size_t l);

#endif
