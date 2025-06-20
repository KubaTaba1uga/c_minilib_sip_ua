#include "udp_socket/udp_socket.h"
#include "udp_socket/_internal/udp_socket.h"
#include "udp_socket/_internal/udp_socket_listen.h"
#include "udp_socket/_internal/udp_socket_send.h"

cme_error_t udp_socket_create(event_loop_t evl, ip_t ip_addr,
                              udp_socket_t *out) {
  return __UdpSocket_create(evl, ip_addr, out);
};

cme_error_t udp_socket_listen(udp_socket_t udp_socket, udp_socket_recvh_t recvh,
                              void *arg) {
  return __UdpSocket_listen(udp_socket, recvh, arg);
};

udp_socket_t udp_socket_ref(udp_socket_t udp_socketp) {
  return __UdpSocket_ref(udp_socketp);
};

void udp_socket_deref(udp_socket_t udp_socketp) {
  __UdpSocket_deref(udp_socketp);
};

cme_error_t udp_socket_send(udp_socket_t udp_socket, ip_t ip_addr,
                            csview_ptr_t bytes) {
  return __UdpSocket_send(udp_socket, ip_addr, bytes);
}
