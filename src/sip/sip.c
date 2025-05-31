#include "sip/sip.h"
#include "sip/_internal/sip.h"

cme_error_t sip_create_session(cmsu_evl_t evl, ip_addr_t ipaddr,
                               sip_session_t *out) {
  return cmsu_SipSession_create(evl, ipaddr, out);
}
