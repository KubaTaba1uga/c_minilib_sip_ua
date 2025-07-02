/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_SESSION_H
#define C_MINILIB_SIP_UA_INT_SIP_SESSION_H
#include <stdlib.h>

#include "c_minilib_error.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/sip_core.h"
#include "utils/ip.h"

/******************************************************************************
 *                               Sip Session                                  *
 ******************************************************************************/
enum SipSessionState {
  SipSessionState_INVITED, //  ----- reject(...)
  //       | accept(...)           |
  //       V                       |
  SipSessionState_ESTABLISHED, //  |
  //       | bye(...)              |
  //       V                       |
  SipSessionState_TERMINATED, // <--
};

struct SipSessionPtr;
typedef cme_error_t (*sip_session_next_stateh_t)(
    enum SipSessionState next_state, struct SipMessagePtr sip_msg,
    struct SipSessionPtr sip_session, struct GenericPtr arg);

struct __SipSession {
  struct SipCorePtr sip_core;
  enum SipSessionState state;
  struct SipServerTransactionPtr current_strans;

  // Sip Session User (SSU) data
  sip_session_next_stateh_t next_stateh;
  struct GenericPtr arg;
};

void __SipSession_destroy(struct __SipSession *sip_core);
static inline struct __SipSession
__SipSession_clone(struct __SipSession sip_core) {
  return sip_core;
};

#define i_type SipSessionPtr
#define i_key struct __SipSession
#define i_keydrop __SipSession_destroy
#define i_keyclone __SipSession_clone
#include "stc/arc.h"

cme_error_t __SipSessionPtr_create(struct SipCorePtr sip_core,
                                   struct SipServerTransactionPtr strans,
                                   sip_session_next_stateh_t next_stateh,
                                   struct GenericPtr arg,
                                   struct SipSessionPtr *out);

#endif // C_MINILIB_SIP_UA_INT_SIP_SESSION_H
