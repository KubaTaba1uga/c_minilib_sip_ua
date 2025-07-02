/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_SESSION_H
#define C_MINILIB_SIP_UA_INT_SIP_SESSION_H
#include <stdlib.h>

#include "c_minilib_error.h"
#include "sip_core/sip_core.h"
#include "utils/ip.h"

/******************************************************************************
 *                               Sip Session                                  *
 ******************************************************************************/
enum SipSessionState {
  SipSessionState_INVITED,
  SipSessionState_ESTABLISHED,
  SipSessionState_TERMINATED,
};

typedef cme_error_t (*sip_session_recvh_t)(enum SipSessionState next_state,
                                           struct SipMessagePtr sip_msg,
                                           struct SipCorePtr sip_session,
                                           struct GenericPtr arg);

struct __SipSession {
  struct SipCorePtr sip_core;
  enum SipSessionState state;

  // Sip Session User (SSU) data
  sip_session_recvh_t recvh;
  struct GenericPtr arg;
};

#endif // C_MINILIB_SIP_UA_INT_SIP_SESSION_H
