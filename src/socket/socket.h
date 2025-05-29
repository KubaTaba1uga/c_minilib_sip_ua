#ifndef C_MINILIB_SIP_UA_SOCKET_H
#define C_MINILIB_SIP_UA_SOCKET_H

#include "c_minilib_error.h"
#include "socket/_internal/socket.h"
#include "utils/error.h"
#include <stdlib.h>

typedef struct cmsu_Socket *cmsu_sock_t;
typedef struct cmsu_SocketUdp *cmsu_sock_udp_t;

static inline int cmsu_Socket_get_fd(cmsu_sock_t socket) {
  if (!socket || !socket->ctx) {
    return -1;
  }

  return socket->get_fd_func(socket->ctx);
}

static inline void cmsu_Socket_destroy(cmsu_sock_t socket) {
  if (!socket) {
    return;
  }

  int fd = cmsu_Socket_get_fd(socket);
  if (fd > 0) {
    close(fd);
  }

  free(socket->ctx);
  free(socket);
}

#endif // C_MINILIB_SIP_UA_SOCKET_H
