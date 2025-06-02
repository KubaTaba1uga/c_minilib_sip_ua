#include "sip/sip.h"
#include "sip/_internal/sip.h"
#include "socket/socket.h"
#include "utils/id.h"
#include <stdint.h>

cme_error_t sip_session_create(evl_t evl, ip_addr_t ipaddr,
                               sip_session_t *out) {
  return cmsu_SipSession_create(evl, ipaddr, out);
}

void sip_session_destroy(void *sipsess) { cmsu_SipSession_destroy(sipsess); };

id_t sip_session_gen_id(sip_session_t sipsess) {
  return cmsu_SipSession_gen_id(sipsess);
};
