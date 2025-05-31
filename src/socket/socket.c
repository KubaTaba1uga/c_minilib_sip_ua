#include "socket/socket.h"
#include "c_minilib_error.h"
#include "socket/_internal/socket.h"
#include "socket/_internal/socket_vec.h"
#include "socket/_internal/udp.h"

cme_error_t socket_udp_create(
    cmsu_evl_t evl, ip_addr_t ipaddr,
    cme_error_t (*recv_callback)(socket_t sock, buffer_t *buf, void *ctx),
    cme_error_t (*send_callback)(socket_t sock, ip_addr_t recver, buffer_t *buf,
                                 void *ctx),
    void(*ctx_destroy), void *ctx, socket_t *out) {

  if (!out) {
    return cme_return(cme_error(EINVAL, "`out` cannot be NULL"));
  }

  return cmsu_SocketUdp_create(evl, ipaddr, ctx, recv_callback, send_callback,
                               ctx_destroy, out);
}

cme_error_t socket_recv(socket_t socket) { return cmsu_Socket_recv(socket); };

cme_error_t socket_send(ip_addr_t recver, buffer_t buf, socket_t socket) {
  return cmsu_Socket_send(recver, &buf, socket);
};

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
