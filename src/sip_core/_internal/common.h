#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_core/_internal/sip_listener.h"
#include "sip_core/_internal/sip_listener_list.h"
#include "sip_core/_internal/sip_strans_hashmap.h"
#include "sip_core/sip_core.h"
#include "sip_transp/sip_transp.h"

struct cmsu_SipCore {
  sip_transp_t sip_transp;
  list_cmsu_SipListeners sip_lstnrs;
  hmap_cmsu_SipStransMap sip_strans;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
