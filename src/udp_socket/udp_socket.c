#include "udp_socket/udp_socket.h"
#include "udp_socket/_internal/udp_socket.h"
#include "udp_socket/_internal/udp_socket_listen.h"

cme_error_t udp_socket_create(event_loop_ptr_t evl, ip_t ip_addr,
                              udp_socket_ptr_t *out) {
  return __UdpSocket_create(evl, ip_addr, out);
};

udp_socket_ptr_t udp_socket_ref(udp_socket_ptr_t udp) {
  return __UdpSocket_ref(udp);
};

void udp_socket_deref(udp_socket_ptr_t *udp) { __UdpSocket_deref(udp); };

cme_error_t udp_socket_listen(udp_socket_ptr_t *udp_socket,
                              udp_socket_recvh_t recvh, void *arg) {
  return __UdpSocket_listen(udp_socket, recvh, arg);
};
