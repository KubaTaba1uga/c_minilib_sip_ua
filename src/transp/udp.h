#ifndef C_MINILIB_SIP_UA_TRANSPORT_UDP_H
#define C_MINILIB_SIP_UA_TRANSPORT_UDP_H

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
#include "socket99.h"

struct cmsu_TransportUdp {
  int sockfd;
  int recv_timerfd;
  int send_timerfd;
  // TO-DO: we should pass some event and received/to send data. Also protocols
  // like sip or rtp may requiire preserving some state so they need sth like
  // callback data.
  cme_error_t (**callbacks)(struct cmsu_TransportUdp *ctx, void *data);
};

typedef struct cmsu_TransportUdp cmsu_TransportUdp;

// This should take ip and port
static inline cme_error_t cmsu_udp_create(struct cmsu_TransportUdp **ctx) {
  cme_error_t err;
  if (!ctx) {
    err = cme_error(EINVAL, "`ctx` cannot be NULL");
    goto error_out;
  }

  *ctx = calloc(1, sizeof(struct cmsu_TransportUdp));
  if (!*ctx) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `ctx`");
    goto error_out;
  }

  int v_true = 1;
  socket99_result res;
  bool ok = socket99_open(
      &(socket99_config){
          .host = "127.0.0.1",
          .port = 7337,
          .server = true,
          .datagram = true,
          .nonblocking = true,
          .sockopts =
              {
                  {SO_REUSEADDR, &v_true, sizeof(v_true)},
              },
      },
      &res);
  if (!ok) {
    err = cme_error(res.saved_errno, "Cannot create socket");
    goto error_ctx_cleanup;
  }

  memset(*ctx, 0, sizeof(struct cmsu_TransportUdp));
  (*ctx)->sockfd = res.fd;

  return 0;

error_ctx_cleanup:
  free(*ctx);
  *ctx = NULL;
error_out:
  return cme_return(err);
};

static inline void *cmsu_udp_clone(void *src) {
  struct cmsu_TransportUdp *dst = calloc(1, sizeof(struct cmsu_TransportUdp));
  if (!dst) {
    return NULL;
  }

  *dst = *((struct cmsu_TransportUdp *)src);

  return dst;
}

static inline cme_error_t cmsu_udp_process_events(const cmsu_PollFd *fd,
                                                  void *ctx_) {
  // Receive data
  struct cmsu_TransportUdp *ctx = ctx_;

  if (fd->revents & POLLIN) {
    struct sockaddr sender;
    socklen_t sender_size;
    char buf[1024];

    errno = 0;
    memset(buf, 0, 1024);

    int32_t received_bytes =
        recvfrom(ctx->sockfd, buf, 1023, MSG_NOSIGNAL, &sender, &sender_size);

    if (received_bytes > 0) {
      printf("Read %d bytes: %s\n", received_bytes, buf);
    } else if (received_bytes == 0) {
      printf("Connection closed by peer\n");
      return 0;
    } else {
      return cme_return(
          cme_error(errno, "Error during reciving data over UDP"));
    }
  }

  return 0;
}

static inline int cmsu_udp_get_fd(void *ctx) {
  return ((struct cmsu_TransportUdp *)ctx)->sockfd;
};

static inline cme_error_t cmsu_udp_send(cmsu_TransportUdp *ctx,
                                        const void *buffer) {
  return 0;
}

#endif
