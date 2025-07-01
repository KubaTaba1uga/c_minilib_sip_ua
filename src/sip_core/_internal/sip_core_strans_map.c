#include "sip_core/_internal/sip_core_strans_map.h"

cme_error_t
SipServerTransactions_insert(csview key, struct SipServerTransactionPtr value,
                             struct hmap__SipServerTransactions *hmap,
                             struct SipServerTransactionPtr *out) {
  struct hmap__SipServerTransactions_value *result =
      hmap__SipServerTransactions_insert_or_assign(hmap, cstr_from_sv(key),
                                                   value)
          .ref;

  if (result && out) {
    *out = result->second;
  }

  return 0;
}

struct SipServerTransactionPtr *
SipServerTransactions_find(csview key,
                           struct hmap__SipServerTransactions *stmap,
                           struct SipServerTransactionPtr *out) {
  cstr cstr_key = cstr_from_sv(key);
  const char *raw_key = cstr_toraw(&cstr_key);

  if (!hmap__SipServerTransactions_contains(stmap, raw_key)) {
    *out = (struct SipServerTransactionPtr){0};
    return NULL;
  }

  *out = *hmap__SipServerTransactions_at_mut(stmap, raw_key);

  return out;
}
