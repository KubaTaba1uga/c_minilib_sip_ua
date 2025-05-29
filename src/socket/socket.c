#include <errno.h>

#include "c_minilib_error.h"
#include "socket/_internal/socket.h"
#include "socket/_internal/socket_list.h"
#include "socket/socket.h"

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

cme_error_t cmsu_Sockets_create(cmsu_sock_list_t *sockets) {
  struct list_cmsu_Sockets *list = malloc(sizeof(struct list_cmsu_Sockets));
  if (!list) {
    return cme_return(cme_error(ENOBUFS, "Cannot allocate `socket`"));
  }

  *list = list_cmsu_Sockets_init();

  *sockets = list;

  return 0;
}

void cmsu_Sockets_destroy(cmsu_sock_list_t *sockets) {
  if (!sockets) {
    return;
  }
}

cme_error_t cmsu_Sockets_push(cmsu_sock_list_t sockets, cmsu_sock_t socket) {
  cmsu_sock_t result = list_cmsu_Sockets_push(sockets, *socket);
  if (!result) {
    return cme_return(cme_error(ENOBUFS, "Cannot push to `sockets`"));
  }

  return 0;
}
