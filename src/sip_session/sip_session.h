/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_SESSION_H
#define C_MINILIB_SIP_UA_SIP_SESSION_H
/*
  This is interface to sip_core module, if you need anything from sip_core
  module, put interface declaration here and implementation in .c. For more
  complex code use _internal directory to not make this module dirty.
 */
#include <stdint.h>

#include "c_minilib_error.h"
#include "sip_core/sip_core.h"

/******************************************************************************
 *                               Sip Session                                  *
 ******************************************************************************/
/*
Sip Session is responsible for handling INVITE request e2e, including:
-                    INVITE:  Create/send initial INVITE with offer (SDP).
- 180 Ringing, 200 OK, etc.:  Handle provisional/final responses.
-              ACK handling:  Send/receive ACKs after 200 OK.
-     Dialog state tracking:  Store Call-ID, tags, route-set, CSeq.
-         Session refreshes:  Handle re-INVITE, UPDATE, session timers.
-               Termination:  Send/receive BYE, clean up media and state.
*/
// TO-DO add user ops and data, so session is usable
cme_error_t SipSessionPtr_listen(struct SipCorePtr sip_core);

#endif // C_MINILIB_SIP_UA_SIP_SESSION_H
