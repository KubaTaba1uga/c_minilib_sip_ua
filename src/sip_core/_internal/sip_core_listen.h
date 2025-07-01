/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#include "sip_core/_internal/sip_core.h"
#include "sip_core/sip_core.h"
#include "sip_transport/sip_transport.h"
#include "utils/ip.h"
#include "utils/sip_msg.h"

/*
 Sip Core Listener allow for handling incoming sip messages. Once new Sip
 message come (which is not part of any existing sip transaction) reqh is
 called. If reqh decide that this transaction should be further processed it
 invokes Sip Core reply and provide status code and status phrase.
 Design looks sth like this:

  Receiving new transaction:
     Another UA
           |
           |  Valid Sip Request (not matching any existing transaction)
           |
           V
     Sip Core Listener
           |
           |  SipMessage, SipServerTransaction
           |
           V
     `sip_core_reqh_t reqh`

   Handling existsing transaction:
     Another UA
           |
           |  Valid Sip Request (matching some existing transaction)
           |
           V
     Sip Core Listener
           |
           |  SipMessage, SipServerTransaction
           |
           V
     Sip Server Transaction

 There are multiple listeners per one sip core so TU can split their ops into
 multiple modules. Like one listener for invites one for registers one for SMSes
 etc. having multiple listeners allow each listener to decide whther take action
 or no in modular fashion.
*/
struct __SipCoreListener {
  sip_core_reqh_t reqh;
  // Strans_Errh is used only by transaction to inform TU about err in
  // transaction
  sip_core_strans_errh_t strans_errh;
  struct GenericPtr arg;
};

#define i_tag _SipCoreListeners
#define i_key struct __SipCoreListener
#include "stc/queue.h"

cme_error_t __SipCore_listen(sip_core_reqh_t reqh, struct GenericPtr arg,
                             struct SipCorePtr sip_core);

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_LISTEN_H
