#ifndef C_MINILIB_SIP_UA_TRANSPORT_H
#define C_MINILIB_SIP_UA_TRANSPORT_H

#include "c_minilib_error.h"

typedef struct {
  int sockfd;
} cmsu_UdpCtx;

typedef struct {
  int sockfd;
} cmsu_TcpCtx;

cme_error_t cmsu_udp_create(cmsu_UdpCtx *ctx);
cme_error_t cmsu_tcp_create(cmsu_TcpCtx *ctx);

#define cmsu_transport_create(ctx)                                             \
  _Generic((ctx), cmsu_UdpCtx *                                                \
           : cmsu_udp_create, cmsu_TcpCtx *                                    \
           : cmsu_tcp_create)(ctx)

cme_error_t cmsu_udp_send(cmsu_UdpCtx *ctx, const void *b, size_t l);
cme_error_t cmsu_tcp_send(cmsu_TcpCtx *ctx, const void *b, size_t l);

#define transport_send(ctx, buf, len)                                          \
  _Generic((ctx), cmsu_UdpCtx *                                                \
           : cmsu_udp_send, cmsu_TcpCtx *                                      \
           : cmsu_tcp_send)(ctx, buf, len)

#endif
