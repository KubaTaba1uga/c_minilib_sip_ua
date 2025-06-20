/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UDP_SEND_H
#define C_MINILIB_SIP_UA_INT_UDP_SEND_H
#include <arpa/inet.h>
#include <asm-generic/errno.h>
#include <assert.h>
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
#include "udp_socket/_internal/common.h"
#include "udp_socket/udp_socket.h"
#include "utils/csview_ptr.h"
#include "utils/ip.h"

static inline cme_error_t __UdpSocket_send(udp_socket_t udp_socket,
                                           ip_t ip_addr, csview_ptr_t bytes) {
  struct addrinfo *receiver_addr = NULL;
  cme_error_t err;

  { // Get receiver address
    struct addrinfo hints = {0};
    int _err;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    _err = getaddrinfo(ip_addr.ip, ip_addr.port, &hints, &receiver_addr);
    if (_err) {
      err = cme_errorf(_err, "Cannot get ip addres for %s:%s", ip_addr.ip,
                       ip_addr.port);
      goto error_out;
    }
  }

  { // Send data to receiver
    errno = 0;
    int32_t sent_bytes =
        sendto(udp_socket->get->fd, bytes.get->buf, bytes.get->size,
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
