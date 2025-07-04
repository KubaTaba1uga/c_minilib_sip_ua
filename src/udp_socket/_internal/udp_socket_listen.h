/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_LISTEN_H
#define C_MINILIB_SIP_UA_INT_UDP_LISTEN_H
#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "stc/cstr.h"
#include "udp_socket/udp_socket.h"
#include "utils/buffer_ptr.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"
#include "utils/memory.h"

typedef cme_error_t (*udp_socket_recvh_t)(struct BufferPtr buf,
                                          struct IpAddrPtr peer,
                                          struct GenericPtr data);

static inline cme_error_t __UdpSocket_listen(struct UdpSocketPtr udp_socketp,
                                             udp_socket_recvh_t recvh,
                                             struct GenericPtr arg) {
  puts(__func__);
  cme_error_t err =
      EventLoopPtr_set_pollin(udp_socketp.get->evl, udp_socketp.get->fd);
  if (err) {
    goto error_out;
  }

  udp_socketp.get->recvh = recvh;
  udp_socketp.get->recvh_arg = arg;

  return 0;

error_out:
  return err;
};

inline static cme_error_t __UdpSocket_recv(struct GenericPtr data) {
  puts(__func__);

  // Ref Udp Socket
  struct UdpSocketPtr udp_socketp = GenericPtr_dump(UdpSocketPtr, data);
  struct sockaddr_storage sender_addr;
  socklen_t sender_addr_len;
  struct BufferPtr buf_ptr;
  struct IpAddrPtr ip;
  int32_t buf_len;
  cme_error_t err;

  assert(udp_socketp.get != NULL);
  assert(udp_socketp.use_count != NULL);
  assert(udp_socketp.get->fd != 0);

  // Ref Buffer
  err = BufferPtr_create_empty(__UDP_MSG_SIZE_MAX, &buf_ptr);
  if (err) {
    goto error_out;
  }

  sender_addr_len = sizeof(sender_addr);

  errno = 0;
  buf_len =
      recvfrom(udp_socketp.get->fd, (void *)buf_ptr.get->buf, buf_ptr.get->size,
               MSG_NOSIGNAL, (struct sockaddr *)&sender_addr, &sender_addr_len);
  if (buf_len < 0) {
    err = cme_error(errno, "Cannot recieve udp data");
    goto error_buf_cleanup;
  } else if (buf_len == 0) {
    err = cme_error(errno, "Connection closed by peer");
    goto error_buf_cleanup;
  }

  buf_ptr.get->size = buf_len;

  if (udp_socketp.get->recvh) {
    char *ip_str[INET6_ADDRSTRLEN + 1];
    char *port_str[8 + 1];

    memset(ip_str, 0, INET6_ADDRSTRLEN + 1);
    memset(port_str, 0, 8 + 1);

    struct sockaddr_in *s = (struct sockaddr_in *)&sender_addr;
    inet_ntop(AF_INET, &s->sin_addr, (char *)ip_str, INET6_ADDRSTRLEN);
    snprintf((char *)port_str, 8, "%u", ntohs(s->sin_port));

    // Ref Ip
    ip = IpAddrPtr_create(cstr_from((const char *)ip_str),
                          cstr_from((const char *)port_str));

    err = udp_socketp.get->recvh(buf_ptr, ip, udp_socketp.get->recvh_arg);
    if (err) {
      goto error_ip_cleanup;
    }

    // Deref Ip
    IpAddrPtr_drop(&ip);
  }

  // Deref Buffer
  BufferPtr_drop(&buf_ptr);

  return 0;

error_ip_cleanup:
  IpAddrPtr_drop(&ip);
error_buf_cleanup:
  BufferPtr_drop(&buf_ptr);
error_out:
  return cme_return(err);
}

#endif
