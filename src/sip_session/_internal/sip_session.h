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

typedef cme_error_t (*sip_session_next_stateh_t)(
    enum SipSessionState next_state, struct SipMessagePtr sip_msg,
    struct SipCorePtr sip_session, struct GenericPtr arg);

struct __SipSession {
  struct SipCorePtr sip_core;
  enum SipSessionState state;

  // Sip Session User (SSU) data
  sip_session_next_stateh_t next_stateh;
  struct GenericPtr arg;
};

static inline void __SipSession_destroy(struct __SipSession *sip_core);
static inline struct __SipSession
__SipSession_clone(struct __SipSession sip_core) {
  return sip_core;
};

#define i_type SipSessionPtr
#define i_key struct __SipSession
#define i_keydrop __SipSession_destroy
#define i_keyclone __SipSession_clone
#include "stc/arc.h"

static inline void __SipSessionPtr_create(struct SipCorePtr sip_core,
                                          sip_session_next_stateh_t next_stateh,
                                          struct GenericPtr arg,
                                          struct SipSessionPtr *out) {
  struct __SipSession *sip_session = my_malloc(sizeof(struct __SipSession));
  cme_error_t err;

  *sip_session = (struct __SipSession){.sip_core = sip_core,
                                       .next_stateh = next_stateh,
                                       .arg = arg,
                                       .state = SipSessionState_INVITED};
};
static inline void __SipSession_destroy(struct __SipSession *sip_core);

#endif // C_MINILIB_SIP_UA_INT_SIP_SESSION_H
