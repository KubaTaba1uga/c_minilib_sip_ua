#include "socket/socket.h"
#include "c_minilib_error.h"
#include "socket/_internal/socket.h"
#include "socket/_internal/socket_vec.h"
#include "socket/_internal/udp.h"
#include <stdbool.h>

cme_error_t socket_udp_create(
    cmsu_evl_t evl, ip_addr_t ipaddr,
    cme_error_t (*recv_callback)(socket_t socket, ip_addr_t *sender,
                                 buffer_t *buf, void *ctx_),
    cme_error_t (*send_callback)(socket_t socket, ip_addr_t *recver,
                                 buffer_t *buf, void *data, void *ctx_),
    void(*ctx_destroy), void *ctx, socket_t *out) {

  if (!out) {
    return cme_return(cme_error(EINVAL, "`out` cannot be NULL"));
  }

  return cmsu_SocketUdp_create(evl, ipaddr, ctx, recv_callback, send_callback,
                               ctx_destroy, out);
}

cme_error_t socket_recv_event_handler(socket_t socket) {
  return cmsu_Socket_recv_event_handler(socket);
};

cme_error_t socket_send_event_handler(socket_t socket, bool *is_send_done) {
  return cmsu_Socket_send_event_handler(socket, is_send_done);
};

cme_error_t socket_fail_event_handler(bool is_err, bool is_conn_close,
                                      socket_t socket) {
  return 0;
};

cme_error_t socket_recv_sync(ip_addr_t *sender, buffer_t *buffer,
                             socket_t socket) {
  return cmsu_Socket_recv_sync(sender, buffer, socket);
};
cme_error_t socket_send_sync(ip_addr_t *recver, buffer_t *buffer,
                             socket_t socket) {
  return cmsu_Socket_send_sync(recver, buffer, socket);
};

int socket_get_fd(socket_t socket) { return cmsu_Socket_get_fd(socket); }

void socket_destroy(socket_t *socket) { cmsu_Socket_destroy(socket); };

cme_error_t vec_socket_create(vec_socket_t *sockets) {
  return cmsu_Sockets_create(sockets);
};

socket_t vec_socket_get(uint32_t fd, vec_socket_t sockets) {
  return cmsu_Sockets_get(fd, sockets);
};

cme_error_t vec_socket_insert(socket_t socket, vec_socket_t sockets) {
  return cmsu_Sockets_insert(socket, sockets);
};

void vec_socket_destroy(vec_socket_t *sockets) {
  cmsu_Sockets_destroy(sockets);
};
