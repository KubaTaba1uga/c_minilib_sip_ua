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
#include "sip_session/_internal/sip_session.h"

/******************************************************************************
 *                               Sip Session                                  *
 ******************************************************************************/
/*
  Sip Session is responsible for handling INVITE request e2e, including:
  -                    INVITE:  Receive/send initial INVITE with offer (SDP).
  - 180 Ringing, 200 OK, etc.:  Handle provisional/final responses.
  -              ACK handling:  Send/receive ACKs after 200 OK.
  -     Dialog state tracking:  Store Call-ID, tags, route-set, CSeq.
  -         Session refreshes:  Handle re-INVITE, UPDATE, session timers.
  -               Termination:  Send/receive BYE, clean up media and state.
*/

/*
  So we model the sip session sth like this:
  - On invite inviteh is run. This is where user decides to send 200 or 486.
  - Let's assume user send 200. On ACK estabh is run. Here user can start media
  transimission if want to.
  - Let's assume user received bye. On BYE byeh is run. Here user can teardown
  media and end call.

  Or in picture:
  Sip Core
  |
  | sip message, peer ip
  V
  Sip Session
  |
  |
  -------------------------------------------------------
  |                  |                   |              |
  | INVITE           | ACK               | BYE          | error
  V                  V                   V              V
  INVITED            ESTABLISHED         TERMINATED     TERMINATED

  Recivin Call in sip looks somethink like this:
  - recive INVITE -- state: INVITED
  - start ringing and issue 180
  - accept call with 200
  - receive ACK -- state: ESTABLISHED
  - establish media
  - exchange media
  - receive/send BYE -- state:TERMINATED
  - teardown media
*/

cme_error_t SipSessionPtr_create(sip_session_recvh_t recvh,
                                 struct GenericPtr arg,
                                 struct SipCorePtr sip_core,
                                 struct SipSessionPtr *out);

cme_error_t SipSessionPtr_accept(struct SipSessionPtr *sip_session);
cme_error_t SipSessionPtr_reject(struct SipSessionPtr *sip_session);

/*
  Starting Call in sip looks somethink like this:
  - send INVITE -- state: INVITED
  - receive 100
  - receive 180
  - receive 200 -- state: ESTABLISHED
  - send ACK
  - establish media
  - exchange media
  - receive/send BYE -- state:TERMINATED
  - teardown media
*/
cme_error_t SipSessionPtr_connect(sip_session_recvh_t recvh,
                                  struct GenericPtr arg,
                                  struct SipCorePtr sip_core,
                                  struct SipSessionPtr *out);

#endif // C_MINILIB_SIP_UA_SIP_SESSION_H
