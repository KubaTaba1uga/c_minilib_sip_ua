#ifndef C_MINILIB_SIP_UA_INT_SOCKET_H
#define C_MINILIB_SIP_UA_INT_SOCKET_H

#include "c_minilib_error.h"
#include "utils/error.h"
#include <unistd.h>

enum cmsu_SupportedSockets {
  cmsu_SupportedSockets_UDP,
};

struct cmsu_Socket {
  // Data
  enum cmsu_SupportedSockets proto;
  void *ctx;
  cme_error_t (*send_clbck)(void *ctx); // This func get called on send event
  cme_error_t (*recv_clbck)(void *ctx); // This func get called on recv event
  int (*get_fd_func)(void *ctx); // Every socket requires at least one fd, we
                                 // use these fd to distinguish among sockets.
};

typedef struct cmsu_Socket cmsu_Socket;

static inline int cmsu_Socket_cmp(const struct cmsu_Socket *a,
                                  const struct cmsu_Socket *b) {
  int a_fd = a->get_fd_func(a->ctx);
  int b_fd = b->get_fd_func(b->ctx);

  if (a_fd == b_fd) {
    return 0;
  }

  if (a_fd > b_fd) {
    return 1;
  }

  return -1;
}

static inline void cmsu_Socket_drop(struct cmsu_Socket *self) {
  close(self->get_fd_func(self->ctx));
  free(self->ctx);
};

#endif // C_MINILIB_SIP_UA_SOCKET_H
