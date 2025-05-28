#ifndef C_MINILIB_SIP_UA_EVENT_LOOP_H
#define C_MINILIB_SIP_UA_EVENT_LOOP_H

#include "c_minilib_error.h"
#include "transp/transp.h"
#include "utils/error.h"

cme_error_t cmsu_event_loop_start(void);

cme_error_t cmsu_event_loop_insert_udp_transp(short revents,
                                              struct cmsu_TransportUdp *transp);

#endif // C_MINILIB_SIP_UA_EVENT_LOOP_H
