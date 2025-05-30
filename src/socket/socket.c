#include "socket/socket.h"
#include "socket/_internal/udp.h"

cme_error_t socket_udp_create(socket_udp_create_arg args, socket_t *out) {
  return cmsu_SocketUdp_create(args, out);
};
