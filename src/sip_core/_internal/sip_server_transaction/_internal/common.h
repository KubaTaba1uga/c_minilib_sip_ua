
static inline cme_error_t
SipServerTransactionPtr_get_branch(struct SipServerTransactionPtr strans,
                                   struct csview *out) {
  cme_error_t err;

  void *result = SipMessagePtr_get_branch(strans.get->init_request, out);
  if (!result) {
    err =
        cme_error(ENODATA, "Missing via->branch in server transaction request");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}
