#include "socket/_internal/socket.h"

int cmsu_Socket_get_fd(struct cmsu_Socket *socket) {
  if (!socket || !socket->ctx) {
    return -1;
  }

  return socket->get_fd_func(socket->ctx);
}

void cmsu_Socket_destroy(struct cmsu_Socket *socket) {
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
