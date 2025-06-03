#include "sip/sip.h"
#include "c_minilib_error.h"
#include "sip/_internal/common.h"
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

cme_error_t sip_stack_connect(sip_stack_t sip_stack, ip_addr_t recver,
                              sip_msg_t msg) {
  if (!cmsu_is_sipmsg_valid(msg)) {
    return cme_return(cme_error(EINVAL, "`msg` is invalid"));
  }
  return cmsu_SipStack_connect(sip_stack, recver, msg);
}
