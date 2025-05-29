/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SOCKET_LIST_H
#define C_MINILIB_SIP_UA_INT_SOCKET_LIST_H

#include "socket/_internal/socket.h"

typedef struct cmsu_Socket cmsu_Socket;

#define i_tag cmsu_Sockets
#define i_key cmsu_Socket
#define i_cmp cmsu_Socket_cmp
#include <stc/list.h>

#endif
