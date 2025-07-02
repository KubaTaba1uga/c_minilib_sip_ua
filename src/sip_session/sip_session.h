/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_SESSION_H
#define C_MINILIB_SIP_UA_SIP_SESSION_H
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

  Sip session is responsible for handling INVITE requests, as consequence it
  needs to handle SIP call singnalling. To use sip session you need to set up
  listener if you want to receive SIP calls or use session connect in case you
  want to initiate outgoing call.

  Sip session is built on top of Sip Core and Sip Server Transaction.
  So user using Sip Session shouldn't need to worry about them. User only need
  to create Sip Core instance and allow sip session to take care of signalling
  part of call, leaving to user the media part.

  Dependencies looks sth like this:

  User
  |
  |-> Sip Core
  |
  |-> Sip Session
      |
      |-> Sip Core
      |
      |-> Sip Core/Server Transaction
*/

/*
  Before User can receive incoming Sip Session, User needs to set up session
  Listener. Session Listener creates session when new INVITE comes and dispatch
  it with INVITED state into `sip_session_next_stateh_t next_stateh` function.

  Then let's assume that during `sip_session_next_stateh_t next_stateh` user
  send's 200. When other UA responds with ACK The Listener dispatch ESTABLISHED
  event to the user.

  Now user is responsible for handling media part of session.

  Once user stop media exchange, user send BYE and we dispatch TERMINATED event
  to `sip_session_next_stateh_t next_stateh`.

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
  |                  |                   |              |
  |                  |                   |              |
  -------------------------------------------------------
  |
  | sip message, sip_sesssion, next_state
  V
  `sip_session_next_stateh_t next_stateh`

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
cme_error_t SipSessionPtr_listen(struct SipCorePtr sip_core,
                                 sip_session_next_stateh_t next_stateh,
                                 struct GenericPtr arg);

cme_error_t SipSessionPtr_accept(struct SipSessionPtr *sip_session);
cme_error_t SipSessionPtr_reject(struct SipSessionPtr *sip_session);
cme_error_t SipSessionPtr_bye(struct SipSessionPtr *sip_session);

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
cme_error_t SipSessionPtr_connect(sip_session_next_stateh_t next_stateh,
                                  struct GenericPtr arg,
                                  struct SipCorePtr sip_core,
                                  struct SipSessionPtr *out);

#endif // C_MINILIB_SIP_UA_SIP_SESSION_H
