#include "udp_socket/udp_socket.h"
#include "udp_socket/_internal/udp_socket.h"

cme_error_t udp_socket_create(event_loop_t evl, ip_t ip_addr,
                              udp_socket_t *out) {
  return __UdpSocket_create(evl, ip_addr, out);
};
udp_socket_t udp_socket_ref(udp_socket_t evl) { return (udp_socket_t){}; };
void udp_socket_deref(udp_socket_t *evl){

};
