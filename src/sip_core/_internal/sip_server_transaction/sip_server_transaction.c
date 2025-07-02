#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "sip_core/_internal/sip_core_strans_map.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction_invite.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"
#include "utils/sip_msg.h"

#include "c_minilib_error.h"

cme_error_t __SipServerTransactionPtr_create(
    struct SipMessagePtr sip_msg, struct SipCorePtr sip_core,
    struct IpAddrPtr peer_ip, sip_core_strans_errh_t errh,
    struct GenericPtr arg, struct SipServerTransactionPtr *out) {
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
      .errh = errh,
      .errh_arg = arg,
  };

  *out = SipServerTransactionPtr_from_ptr(strans);

  err = __SipServerTransactionPtr_recvh(sip_msg, peer_ip, out);
  if (err) {
    goto error_out_cleanup;
  }

  csview strans_id = {0};
  err = __SipServerTransactionPtr_get_id(*out, &strans_id);
  if (err) {
    goto error_out_cleanup;
  }

  err = __SipServerTransactions_insert(strans_id, *out, sip_core.get->stranses,
                                       NULL);
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
__SipServerTransactionPtr_reply(uint32_t status_code, cstr status_phrase,
                                struct SipServerTransactionPtr *strans) {
  cme_error_t err;

  switch (strans->get->type) {
  case __SipServerTransactionType_INVITE:
    return __SipServerTransactionPtr_invite_reply(status_code, status_phrase,
                                                  *strans);
  case __SipServerTransactionType_NONINVITE:
    err = cme_error(
        ENOENT, "Non-invite server transactions are currently not supported");
    goto error_out;
  }

error_out:
  return cme_return(err);
}

cme_error_t
__SipServerTransactionPtr_get_id(struct SipServerTransactionPtr strans,
                                 struct csview *out) {
  return 0;
};

cme_error_t
__SipServerTransactionPtr_recvh(struct SipMessagePtr sip_msg,
                                struct IpAddrPtr peer_ip,
                                struct SipServerTransactionPtr *strans) {
  cme_error_t err;

  switch (strans->get->type) {
  case __SipServerTransactionType_INVITE:
    return __SipServerTransactionPtr_invite_recv(sip_msg, peer_ip, strans);
  case __SipServerTransactionType_NONINVITE:
    err = cme_error(
        ENOENT, "Non-invite server transactions are currently not supported");
    goto error_out;
  }

error_out:
  return cme_return(err);
}

bool __SipServerTransactionPtr_is_responses_empty(
    struct SipServerTransactionPtr strans) {
  return list__SipServerTransactionResponses_is_empty(
      &strans.get->sip_responses);
}
