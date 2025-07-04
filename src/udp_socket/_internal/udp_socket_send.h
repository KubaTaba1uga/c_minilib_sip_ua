/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_SEND_H
#define C_MINILIB_SIP_UA_INT_UDP_SEND_H
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
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
#include "utils/ip.h"

static inline cme_error_t __UdpSocket_send(struct UdpSocketPtr udp_socket,
                                           struct IpAddrPtr ip_addr,
                                           struct BufferPtr bytes) {
  puts(__func__);
  struct addrinfo *receiver_addr = NULL;
  cme_error_t err;

  { // Get receiver address
    struct addrinfo hints = {0};
    int _err;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    _err = getaddrinfo(cstr_str(&ip_addr.get->ip), cstr_str(&ip_addr.get->port),
                       &hints, &receiver_addr);
    if (_err) {
      err = cme_errorf(_err, "Cannot get ip addres for %s:%s", ip_addr.get->ip,
                       ip_addr.get->port);
      goto error_out;
    }
  }

  { // Send data to receiver
    errno = 0;
    int32_t sent_bytes =
        sendto(udp_socket.get->fd, bytes.get->buf, bytes.get->size,
               MSG_NOSIGNAL, receiver_addr->ai_addr, receiver_addr->ai_addrlen);
    if (sent_bytes < 0) {
      err = cme_error(errno, "Cannot send udp data");
      goto error_receiver_addr_cleanup;
    } else if (sent_bytes == 0) {
      err = cme_error(ECONNABORTED, "Connection clsed by peer");
      goto error_receiver_addr_cleanup;
    }

    assert(sent_bytes == bytes.get->size);
  }

  freeaddrinfo(receiver_addr);

  return 0;

error_receiver_addr_cleanup:
  freeaddrinfo(receiver_addr);
error_out:
  return err;
};

#endif // C_MINILIB_SIP_UA_INT_UDP_SEND_H
