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

typedef struct cmsu_Socket *cmsu_sock_t;
typedef struct cmsu_SocketUdp *cmsu_sock_udp_t;

int cmsu_Socket_get_fd(cmsu_sock_t socket);
void cmsu_Socket_destroy(cmsu_sock_t socket);

#endif // C_MINILIB_SIP_UA_SOCKET_H
