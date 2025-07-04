#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <assert.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core_strans.h"
#include "sip_core/_internal/sip_core_strans_invite.h"
#include "sip_core/_internal/sip_core_strans_map.h"
#include "sip_transport/sip_transport.h"
#include "timer_fd/_internal/timer_fd.h"
#include "timer_fd/timer_fd.h"
#include "utils/generic_ptr.h"
#include "utils/memory.h"

/* How server transaction works for INVITE flow? */

/*
 Server transaction starts in NONE state. Point of this state is to send 100
 trying response to the calle. Once 100 is send transaction move to PROCEEDING
 state.

 In PROCEEDING state we need to ensure all responses send by TU are passed to
 transport layer. Once user send 2xx in PROCEEDING state transaction move to
 TERMINATED state. Once user send from 300 to 699 in PROCEEDING state
 transaction move to COMPLETED state, now timer G is set up for T1.

 In TERMINATED state transaction get destructed and all timers are stopped.

 In COMPLETED state timer H is set for T1*64. This timer retransmit last
 response until TERMINATED state is reached. If ACK is received in COMPLETED
 state transaction move to CONFIRMED state.

 In CONFIRMED state transaction absorbs any additional ACK without informing
 User. When CONFIRMED state is entered timer I is set up for T4. Once timer I
 fires up transaction move to TERMINATED state.
 */

cme_error_t SipServerTransactionPtr_create(
    struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct IpAddrPtr peer_ip, struct SipServerTransactionPtr *out) {
  bool is_invite = false;
  csview branch = {0};
  csview method = {0};
  cme_error_t err;

  void *result = SipMessagePtr_get_branch(sip_msg, &branch);
  if (!result) {
    err =
        cme_error(ENODATA, "Missing via->branch in server transaction request");
    goto error_out;
  }

  result = SipServerTransactions_find(branch, sip_core.get->stranses);
  if (result) {
    out = result;
    // Update Ip address
    IpAddrPtr_drop(&out->get->last_peer_ip);
    out->get->last_peer_ip = IpAddrPtr_clone(peer_ip);
    return 0;
  }

  result = SipMessagePtr_get_method(sip_msg, &method);
  if (!result) {
    err = cme_error(
        ENODATA,
        "Missing request_line->sip_method in server transaction request");
    goto error_out;
  }

  is_invite = strncmp(method.buf, "INVITE", method.size) == 0;

  struct __SipServerTransaction *strans =
      my_malloc(sizeof(struct __SipServerTransaction));

  *strans = (struct __SipServerTransaction){
      .type = is_invite ? __SipServerTransactionType_INVITE
                        : __SipServerTransactionType_NONINVITE,
      .state = __SipServerTransactionState_NONE,
      .request = SipMessagePtr_clone(sip_msg),
      .sip_core = SipCorePtr_clone(sip_core),
      .last_peer_ip = IpAddrPtr_clone(peer_ip),
  };

  err = SipServerTransactions_insert(branch,
                                     SipServerTransactionPtr_from_ptr(strans),
                                     sip_core.get->stranses, out);
  if (err) {
    goto error_stransp_cleanup;
  }

  return 0;

error_stransp_cleanup:
  SipServerTransactionPtr_drop(out);
error_out:
  return cme_return(err);
}

void __SipServerTransaction_destroy(void *data) {
  puts(__func__);
  struct SipServerTransactionPtr *sip_strans = data;

  SipMessagePtr_drop(&sip_strans->get->request);
};

cme_error_t
SipServerTransactionPtr_next_state(struct SipMessagePtr sip_msg,
                                   struct SipServerTransactionPtr strans) {
  cme_error_t err;

  switch (strans.get->type) {
  case __SipServerTransactionType_INVITE:
    return SipServerTransactionPtr_invite_next_state(sip_msg, strans);
  case __SipServerTransactionType_NONINVITE:
    err = cme_error(
        ENOENT, "Non invite server transactions are currently not supported");
    goto error_out;
  }

error_out:
  return cme_return(err);
}

cme_error_t
SipServerTransactionPtr_reply(uint32_t status_code, cstr status,
                              struct SipServerTransactionPtr strans) {
  puts(__func__);
  struct BufferPtr bytes;
  struct SipMessagePtr sipmsg;
  cme_error_t err;

  err = SipMessagePtr_status_from_request(strans.get->request, status_code,
                                          status, &sipmsg);
  if (err) {
    goto error_out;
  }

  err = SipMessagePtr_generate(sipmsg, &bytes);
  if (err) {
    goto error_sip_msg_cleanup;
  }

  err = SipTransportPtr_send(&strans.get->sip_core.get->sip_transp,
                             strans.get->last_peer_ip, bytes);
  if (err) {
    goto error_bytes_cleanup;
  }

  switch (strans.get->type) {
  case __SipServerTransactionType_INVITE:
    return SipServerTransactionPtr_invite_reply_callback(sipmsg, strans);
  case __SipServerTransactionType_NONINVITE:
    err = cme_error(
        ENOENT, "Non invite server transactions are currently not supported");
    goto error_bytes_cleanup;
  }

  BufferPtr_drop(&bytes);
  SipMessagePtr_drop(&sipmsg);

  return 0;

error_bytes_cleanup:
  BufferPtr_drop(&bytes);
error_sip_msg_cleanup:
  SipMessagePtr_drop(&sipmsg);
error_out:
  return cme_return(err);
}
