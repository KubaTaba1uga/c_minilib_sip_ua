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
void cmsu_sock_destroy(cmsu_sock_t *socket);
int cmsu_sock_get_fd(cmsu_sock_t socket);
cme_error_t cmsu_sock_recv(cmsu_sock_t socket);
cme_error_t cmsu_sock_send(cmsu_sock_t socket);
cme_error_t cmsu_sock_list_create(cmsu_sock_list_t *sockets);
cme_error_t cmsu_sock_list_insert_udp(const char *ipaddr, uint32_t port,
                                      cmsu_sock_t *out,
                                      cmsu_sock_list_t sockets);
cmsu_sock_t cmsu_sock_list_find(int fd, cmsu_sock_list_t sockets);
void cmsu_sock_list_destroy(cmsu_sock_list_t *sockets);

#endif // C_MINILIB_SIP_UA_SOCKET_H
