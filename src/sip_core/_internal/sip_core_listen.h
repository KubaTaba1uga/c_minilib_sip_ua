/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "sip_core/_internal/common.h"
#include "sip_core/sip_core.h"

static inline cme_error_t __SipCore_sip_transp_recvh(sip_msg_t sip_msg,
                                                     ip_t peer_ip,
                                                     sip_transp_t sip_transp,
                                                     void *data) {
  /*
    On every request we do:
     1. If it is sip request (which is not ACK and is not matching to any
current server tramsaction) we are creating new server transaction and running
user callback with this new transaction.

     2. If it is sip request which is matching to any current server tramsaction
we are running user callback whith exsisting server transaction.

     3. If it is sip request ACK we are looking for server transaction that it
matches to to change it state to DONE. We are not running any user
callbacks.

     4. If it is sip status we are looking for client transaction that it
matches to. We then run callback for client_transaction rather than listener.
This means we need sth to match client transactions and user callbacks.
   */

  struct __SipCore *sip_core = data;

  (void)data;

  return 0;

  /* error_out: */
  /*   return cme_return(err); */
}

#endif // C_MINILIB_SIP_UA_INT_SIP_TRANSP_H
