#include "udp/udp.h"
#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "udp/_internal/udp.h"
#include "utils/ip.h"
#include <stdlib.h>

cme_error_t udp_create(event_loop_t evl, ip_t ip_addr, udp_socket_t *out) {

  return cmsu_UdpSocket_create(evl, ip_addr, out);
};

void udp_destroy(udp_socket_t *out) { cmsu_UdpSocket_destroy(out); }

cme_error_t udp_listen(udp_socket_t udp_socket, udp_recvh_t recvh, void *arg) {
  return cmsu_UdpSocket_listen(void *data)
}
