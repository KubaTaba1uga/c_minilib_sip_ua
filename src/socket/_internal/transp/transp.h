#ifndef C_MINILIB_SIP_UA_TRANSPORT_H
#define C_MINILIB_SIP_UA_TRANSPORT_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "c_minilib_error.h"

#include "event_loop/poll/pollfds.h"
#include "transp/udp.h"

enum cmsu_SupportedTransport {
  cmsu_SupportedTransport_UDP,
};

struct cmsu_Transport {
  // Data
  enum cmsu_SupportedTransport type;
  void *ctx;

  // Ops
  cme_error_t (*process_events_func)(const cmsu_PollFd *fd, void *ctx);
  int (*get_fd_func)(
      void *ctx); // Ecery transport needs some socket file descriptor
  void *(*clone_func)(void *src);
};

typedef struct cmsu_Transport cmsu_Transport;

static inline int cmsu_Transport_cmp(const struct cmsu_Transport *a,
                                     const struct cmsu_Transport *b) {
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

static inline struct cmsu_Transport
cmsu_Transport_clone(struct cmsu_Transport src) {
  return (struct cmsu_Transport){.type = src.type,
                                 .ctx = src.clone_func(src.ctx),
                                 .get_fd_func = src.get_fd_func,
                                 .clone_func = src.clone_func};
}

static inline void cmsu_Transport_drop(struct cmsu_Transport *self) {
  close(self->get_fd_func(self->ctx));
  free(self->ctx);
};

#define i_tag cmsu_Transports
#define i_keyclass cmsu_Transport
#define i_cmp cmsu_Transport_cmp
#include <stc/list.h>

typedef struct list_cmsu_Transports cmsu_Transports;

static inline cme_error_t
cmsu_Transports_insert_udp(void *ctx, cmsu_Transports *transports) {
  cme_error_t err;
  if (!ctx || !transports) {
    err = cme_error(EINVAL, "`ctxs` cannot be NULL");
    goto error_out;
  }

  struct cmsu_Transport *result = list_cmsu_Transports_push_back(
      transports,
      (struct cmsu_Transport){.ctx = ctx,
                              .type = cmsu_SupportedTransport_UDP,
                              .process_events_func = cmsu_udp_process_events,
                              .clone_func = cmsu_udp_clone,
                              .get_fd_func = cmsu_udp_get_fd});
  if (!result) {
    err = cme_error(ENOMEM, "Can't add new `udp_ctx` to `transports` list");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline struct cmsu_Transport *
cmsu_Transports_find(int fd, cmsu_Transports transports) {
  struct cmsu_Transport *local;
  c_foreach(transp, list_cmsu_Transports, transports) {
    local = transp.ref;
    if (fd == local->get_fd_func(local->ctx)) {
      return local;
    }
  }

  return NULL;
}

#endif
