#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_H

#include "c_minilib_error.h"
#include "socket/socket.h"
#include "utils/error.h"

cme_error_t cmsu_event_loop_start(void);

cme_error_t cmsu_event_loop_insert_udp_socket(cmsu_sock_udp_t socket);

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
