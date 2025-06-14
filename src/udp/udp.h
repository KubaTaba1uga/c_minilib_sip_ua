/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UDP_H
#define C_MINILIB_SIP_UA_UDP_H
/*
  This is interface to udp module, if you need anything from udp
  module, put interface declaration here and interface implementation in .c but
  always write real implementation as static inline in _internal.
 */

#include <stdint.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "utils/ip.h"

/******************************************************************************
 *                             Event Loop                                     *
 ******************************************************************************/
typedef struct cmsu_UdpSocket *udp_socket_t;
typedef cme_error_t (*udp_recvh_t)(char *buffer, int32_t buffer_len, ip_t peer,
                                   void *socket, void *data);

cme_error_t udp_create(event_loop_t evl, ip_t ip_addr, udp_socket_t *out);
void udp_destroy(udp_socket_t *out);
cme_error_t udp_listen(udp_socket_t udp_socket, udp_recvh_t recvh, void *arg);

#endif // C_MINILIB_SIP_UA_UDP_H
