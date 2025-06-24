#include "udp_socket/_internal/udp_socket.h"

MOCKABLE(cme_error_t __UdpSocket_create_fd(struct IpAddrPtr ip_addr,
                                           int32_t *fd)) {
  errno = 0;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  cme_error_t err;

  if (sockfd == -1) {
    err = cme_errorf(errno, "Cannot create udp socket for IP=%s:%s",
                     ip_addr.get->ip, ip_addr.get->port);
    goto error_out;
  }

  int optval = 1;
  errno = 0;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
      -1) {
    err = cme_errorf(errno, "Cannot configure udp socket for IP=%s:%s",
                     ip_addr.get->ip, ip_addr.get->port);
    goto error_out;
  }

  struct sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  errno = 0;
  my_addr.sin_port = htons(atoi(cstr_data(&ip_addr.get->port)));
  if (inet_pton(AF_INET, cstr_data(&ip_addr.get->ip), &my_addr.sin_addr) != 1) {
    err = cme_errorf(errno, "Cannot parse IP=%s:%s", ip_addr.get->ip,
                     ip_addr.get->port);
    goto error_out;
  }

  errno = 0;
  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
    err = cme_errorf(errno, "Cannot bind udp socket to IP=%s:%s",
                     ip_addr.get->ip, ip_addr.get->port);
    goto error_out;
  }

  *fd = sockfd;

  return 0;

error_out:
  close(sockfd);
  return cme_return(err);
}
