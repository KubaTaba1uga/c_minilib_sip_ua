/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UDP_SOCKET_H
#define C_MINILIB_SIP_UA_UDP_SOCKET_H
#include <poll.h>
#include <stdint.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "stc/cstr.h"

#include "event_loop/event_loop.h"
#include "utils/buffer_ptr.h"
#include "utils/ip.h"

#include "udp_socket/_internal/udp_socket.h"

/******************************************************************************
 *                             Udp Socket                                     *
 ******************************************************************************/
#define i_type UdpSocketPtr
#define i_key struct __UdpSocket
#define i_keydrop __UdpSocket_destroy
#define i_keyclone __UdpSocket_clone
#include "stc/arc.h"

cme_error_t UdpSocketPtr_create(struct EventLoopPtr evlp,
                                struct IpAddrPtr ip_addr,
                                struct UdpSocketPtr *out);

cme_error_t UdpSocketPtr_listen(struct UdpSocketPtr udp_socket,
                                udp_socket_recvh_t recvh,
                                struct GenericPtr arg);

cme_error_t UdpSocketPtr_send(struct UdpSocketPtr udp_socket,
                              struct IpAddrPtr ip_addr, struct BufferPtr bytes);

#undef UdpSocketPtr_from

#endif // C_MINILIB_SIP_UA_UDP_SOCKET_H
