/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SOCK_COMMON_H
#define C_MINILIB_SIP_UA_INT_SOCK_COMMON_H
#include "socket/socket.h"

struct cmsu_Socket {
  // Data
  enum SocketType type;
  void *proto;
};

#endif // C_MINILIB_SIP_UA_SOCK_COMMON_H
