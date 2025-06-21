#include "udp_socket/udp_socket.h"
#include "udp_socket/_internal/udp_socket.h"
#include "udp_socket/_internal/udp_socket_listen.h"
#include "udp_socket/_internal/udp_socket_send.h"
#include "utils/generic_ptr.h"

cme_error_t UdpSocketPtr_create(struct EventLoopPtr evl, ip_t ip_addr,
                                struct UdpSocketPtr *out) {
  cme_error_t err;

  errno = 0;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == -1) {
    err = cme_errorf(errno, "Cannot create udp socket for IP=%s:%s", ip_addr.ip,
                     ip_addr.port);
    goto error_out;
  }

  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
      -1) {
    err = cme_errorf(errno, "Cannot configure udp socket for IP=%s:%s",
                     ip_addr.ip, ip_addr.port);
    goto error_out;
  }

  struct sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(atoi(ip_addr.port));
  int result = inet_pton(AF_INET, ip_addr.ip, &my_addr.sin_addr);
  if (result != 1) {
    err = cme_errorf(errno, "Cannot parse IP=%s:%s", ip_addr.ip, ip_addr.port);
    goto error_out;
  }

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
    err = cme_errorf(errno, "Cannot bind udp socket to IP=%s:%s", ip_addr.ip,
                     ip_addr.port);
    goto error_out;
  }

  *out = UdpSocketPtr_from((struct __UdpSocket){
      .evl = EventLoopPtr_clone(evl), .ip_addr = ip_addr, .fd = sockfd});

  err = EventLoopPtr_insert_socketfd(evl, sockfd, __UdpSocket_recv,
                                     GenericPtr_from(UdpSocketPtr, out));
  if (err) {
    err = cme_errorf(errno,
                     "Cannot insert udp socket into event loop for IP=%s:%s",
                     ip_addr.ip, ip_addr.port);
    goto error_out;
  }

  return 0;

error_out:
  close(sockfd);
  return err;
};

void __UdpSocket_destroy(struct __UdpSocket *udp_socket) {
  // Remove fd from event loop
  EventLoopPtr_remove_fd(udp_socket->evl, udp_socket->fd);
  EventLoopPtr_drop(&udp_socket->evl);

  // Close socket file descriptor
  close(udp_socket->fd);
};

struct __UdpSocket __UdpSocket_clone(struct __UdpSocket udp_socket) {
  return udp_socket;
};

cme_error_t UdpSocketPtr_listen(struct UdpSocketPtr udp_socket,
                                udp_socket_recvh_t recvh,
                                struct GenericPtr arg) {
  return __UdpSocket_listen(udp_socket, recvh, arg);
};

cme_error_t UdpSocketPtr_send(struct UdpSocketPtr udp_socket, ip_t ip_addr,
                              csview_ptr_t bytes) {
  return __UdpSocket_send(udp_socket, ip_addr, bytes);
}
