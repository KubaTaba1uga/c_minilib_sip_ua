#ifndef C_MINILIB_SIP_UA_SIP_TRANSACTION_LIST_H
#define C_MINILIB_SIP_UA_SIP_TRANSACTION_LIST_H

#include "c_minilib_error.h"
#include "sip/_internal/sip_transaction.h"
#include "sip/sip.h"

#define i_tag cmsu_SipTransactions
#define i_key struct cmsu_SipTransaction
#define i_cmp cmsu_SipTransaction_cmp
#include <stc/list.h>

/* static inline cme_error_t */
/* cmsu_SipTransactions_insert(struct cmsu_SipTransaction siptrans, */
/*                             list_cmsu_SipTransactions *list) { */
/*   struct cmsu_SipTransaction *out = */
/*       list_cmsu_SipTransactions_push(&siptrans.stack->transactions,
 * siptrans); */
/*   if (!out) { */
/*   } */
/* }; */

#endif
