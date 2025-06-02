#include "sip/sip.h"
#include "sip/_internal/sip.h"
#include "socket/socket.h"
#include "utils/id.h"
#include <stdint.h>

cme_error_t sip_stack_create(evl_t evl, ip_addr_t ipaddr, sip_stack_t *out) {
  return cmsu_SipStack_create(evl, ipaddr, out);
}

void sip_stack_destroy(void *sipsess) { cmsu_SipStack_destroy(sipsess); };

id_t sip_stack_gen_id(sip_stack_t sipsess) {
  return cmsu_SipStack_gen_id(sipsess);
};
