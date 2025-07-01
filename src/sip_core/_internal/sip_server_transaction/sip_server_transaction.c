#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction_invite.h"
#include "utils/ip.h"
#include "utils/sip_msg.h"

#include "c_minilib_error.h"

static cme_error_t
__SipServerTransactionPtr_recvh(struct SipMessagePtr sip_msg,
                                struct SipServerTransactionPtr *strans);

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
  csview method = {0};
  cme_error_t err;

  void *result = SipMessagePtr_get_method(sip_msg, &method);
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
      .init_request = SipMessagePtr_clone(sip_msg),
      .sip_core = SipCorePtr_clone(sip_core),
      .last_peer_ip = IpAddrPtr_clone(peer_ip),
      .sip_responses = list__SipServerTransactionResponses_init(),
  };

  *out = SipServerTransactionPtr_from_ptr(strans);

  err = __SipServerTransactionPtr_recvh(sip_msg, out);
  if (err) {
    goto error_out_cleanup;
  }

  return 0;

error_out_cleanup:
  SipServerTransactionPtr_drop(out);
error_out:
  return cme_return(err);
}

void __SipServerTransaction_destroy(struct __SipServerTransaction *sip_strans) {
  puts(__func__);

  IpAddrPtr_drop(&sip_strans->last_peer_ip);
  SipCorePtr_drop(&sip_strans->sip_core);

  SipMessagePtr_drop(&sip_strans->init_request);
  list__SipServerTransactionResponses_drop(&sip_strans->sip_responses);
};

cme_error_t
SipServerTransactionPtr_reply(uint32_t status_code, cstr status_phrase,
                              struct SipServerTransactionPtr strans) {
  cme_error_t err;

  switch (strans.get->type) {
  case __SipServerTransactionType_INVITE:
    return __SipServerTransactionPtr_invite_reply(status_code, status_phrase,
                                                  strans);
  case __SipServerTransactionType_NONINVITE:
    err = cme_error(
        ENOENT, "Non-invite server transactions are currently not supported");
    goto error_out;
  }

error_out:
  return cme_return(err);
}

cme_error_t
SipServerTransactionPtr_get_id(struct SipServerTransactionPtr strans,
                               struct csview *out) {
  return 0;
};

cme_error_t
__SipServerTransactionPtr_recvh(struct SipMessagePtr sip_msg,
                                struct SipServerTransactionPtr *strans) {
  cme_error_t err;

  switch (strans->get->type) {
  case __SipServerTransactionType_INVITE:
    return __SipServerTransactionPtr_invite_recv(sip_msg, *strans);
  case __SipServerTransactionType_NONINVITE:
    err = cme_error(
        ENOENT, "Non-invite server transactions are currently not supported");
    goto error_out;
  }

error_out:
  return cme_return(err);
}
