cme_error_t __SipCoreStransMap_insert(csview key,
                                      struct SipServerTransactionPtr strans,
                                      __SipCoreStransMap *stmap,
                                      struct SipServerTransactionPtr *out) {

  struct SipServerTransactionPtr result =
      hmap__SipCoreStransMap_insert_or_assign(stmap, cstr_from_sv(key), strans)
          .ref->second;
  cme_error_t err;

  if (!result) {
    err = cme_error(ENOMEM, "Cannot insert new server transaction to sip core");
    goto error_out;
  }

  *out = result;

  return 0;

error_out:
  __SipCoreStrans_deref(strans);
  *out = NULL;
  return cme_return(err);
}

struct SipServerTransactionPtr
__SipCoreStransMap_find(csview key, __SipCoreStransMap *stmap) {
  cstr cstr_key = cstr_from_sv(key);
  const char *raw_key = cstr_toraw(&cstr_key);

  if (!hmap__SipCoreStransMap_contains(stmap, raw_key)) {
    return NULL;
  }

  return *hmap__SipCoreStransMap_at_mut(stmap, raw_key);
}
