#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#include <stdlib.h>

#include "sip_core/_internal/sip_listener_list.h"
#include "sip_core/_internal/sip_strans_hashmap.h"

struct cmsu_SipCore {
  event_loop_t evl;
  sip_transp_t sip_transp;
  list_cmsu_SipListeners sip_lstnrs;
  hmap_cmsu_SipStransMap sip_strans;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
