#include "udp/udp.h"
#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "utils/ip.h"

cme_error_t udp_create(event_loop_t evl, ip_t ip_addr, udp_socket_t *out) {
  return 0;
};

void udp_destroy(udp_socket_t *out) {}

cme_error_t udp_listen(udp_socket_t *udp_socket, udp_recvh_t recvh, void *arg) {
  return 0;
}
