#include "sip/sip.h"
#include "sip/_internal/sip.h"
#include "utils/id.h"

cme_error_t sip_create_session(cmsu_evl_t evl, ip_addr_t ipaddr,
                               sip_session_t *out) {
  return cmsu_SipSession_create(evl, ipaddr, out);
}

void sip_destroy_session(void *sipsess) { cmsu_SipSession_destroy(sipsess); };

id_t sip_session_gen_id(sip_session_t sipsess) {
  return cmsu_SipSession_gen_id(sipsess);
};
