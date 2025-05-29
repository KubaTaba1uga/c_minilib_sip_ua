#ifndef C_MINILIB_SIP_UA_SOCKET_H
#define C_MINILIB_SIP_UA_SOCKET_H
/*
  This is interface to socket module, if you need anything from socket module,
   put here and in .c interface and write implementation as static inline in
   _internal.
 */

#include <stdlib.h>

#include "c_minilib_error.h"
#include "utils/error.h"

// Socket protos
typedef struct cmsu_Socket *cmsu_sock_t;
typedef struct cmsu_SocketUdp *cmsu_sock_udp_t;

// Socket datastructures
typedef struct list_cmsu_Sockets *cmsu_sock_list_t;

// Ops
cme_error_t cmsu_sock_list_create(cmsu_sock_list_t *sockets);
cme_error_t cmsu_sock_list_push(cmsu_sock_list_t sockets, cmsu_sock_t socket);
#endif // C_MINILIB_SIP_UA_SOCKET_H
