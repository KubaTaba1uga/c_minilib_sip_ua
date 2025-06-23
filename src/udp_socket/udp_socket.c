#include "udp_socket/udp_socket.h"
#include "c_minilib_error.h"
#include "stc/cstr.h"
#include "udp_socket/_internal/udp_socket.h"
#include "udp_socket/_internal/udp_socket_listen.h"
#include "udp_socket/_internal/udp_socket_send.h"
#include "utils/generic_ptr.h"
#include "utils/memory.h"
#include <stdint.h>

cme_error_t UdpSocketPtr_create(struct EventLoopPtr evl,
                                struct IpAddrPtr ip_addr,
                                struct UdpSocketPtr *out) {
  puts(__func__);
  cme_error_t err;
  int32_t sockfd;

  // File descriptor init
  err = __UdpSocket_create_fd(ip_addr, &sockfd);
  if (err) {
    goto error_out;
  }

  struct __UdpSocket *udp_socket = my_malloc(sizeof(struct __UdpSocket));
  *udp_socket = (struct __UdpSocket){
      // Ip Addr init
      .ip_addr = IpAddrPtr_clone(ip_addr),
      // Event Loop init
      .evl = EventLoopPtr_clone(evl),
      .fd = sockfd,
  };

  // Now out->use_count is 1
  *out = UdpSocketPtr_from_ptr(udp_socket);

  err = EventLoopPtr_insert_socketfd(evl, sockfd, __UdpSocket_recv,
                                     GenericPtr_from_arc(UdpSocketPtr, out));
  if (err) {
    err = cme_errorf(errno,
                     "Cannot insert udp socket into event loop for IP=%s:%s",
                     ip_addr.get->ip, ip_addr.get->port);
    goto error_udp_socket_cleanup;
  }

  return 0;

error_udp_socket_cleanup:
  UdpSocketPtr_drop(out);
error_out:
  return cme_return(err);
};

void __UdpSocket_destroy(struct __UdpSocket *udp_socket) {
  puts(__func__);

  // Event loop cleanup
  EventLoopPtr_remove_fd(udp_socket->evl, udp_socket->fd);
  EventLoopPtr_drop(&udp_socket->evl);

  // Ip addr cleanup
  IpAddrPtr_drop(&udp_socket->ip_addr);

  // File descriptor cleanup
  close(udp_socket->fd);
};

cme_error_t UdpSocketPtr_listen(struct UdpSocketPtr udp_socket,
                                udp_socket_recvh_t recvh,
                                struct GenericPtr arg) {
  puts(__func__);
  return __UdpSocket_listen(udp_socket, recvh, arg);
};

cme_error_t UdpSocketPtr_send(struct UdpSocketPtr udp_socket,
                              struct IpAddrPtr ip_addr,
                              struct BufferPtr bytes) {
  puts(__func__);
  return __UdpSocket_send(udp_socket, ip_addr, bytes);
}
