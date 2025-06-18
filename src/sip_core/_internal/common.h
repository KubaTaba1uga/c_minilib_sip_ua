/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#include <stdint.h>

#include "c_minilib_error.h"
#include "sip_core/sip_core.h"
#include "utils/sip_msg.h"

struct __SipCoreListener {
  sip_core_request_handler_t request_handler;
  void *arg;
};

#define i_tag _SipCoreListenersQueue
#define i_key struct __SipCoreListener
#include "stc/queue.h"

typedef struct queue__SipCoreListenersQueue __SipCoreListenersQueue;

struct __SipCore {
  // Transp Protocol data
  event_loop_t evl;
  sip_transp_t sip_transp;

  // SIP core data
  __SipCoreListenersQueue listeners;
};

static inline void __SipCore_destroy(void *data) {
  struct __SipCore *sip_core = data;

  queue__SipCoreListenersQueue_drop(&sip_core->listeners);
  sip_transp_deref(sip_core->sip_transp);
  event_loop_deref(sip_core->evl);
};

static inline struct __SipCore __SipCore_clone(struct __SipCore sip_core) {
  return sip_core;
};

#define i_type __SipCorePtr
#define i_key struct __SipCore
#define i_keydrop __SipCore_destroy
#define i_keyclone __SipCore_clone
#include "stc/arc.h"

struct __SipCoreStrans {
  sip_msg_t request;
};

static inline void __SipCoreStrans_destroy(void *data) {
  struct __SipCoreStrans *sip_strans = data;

  sip_msg_deref(&sip_strans->request);
};

static inline struct __SipCoreStrans
__SipCoreStrans_clone(struct __SipCoreStrans sip_strans) {
  return sip_strans;
};

#define i_type __SipCoreStransPtr
#define i_key struct __SipCoreStrans
#define i_keydrop __SipCoreStrans_destroy
#define i_keyclone __SipCoreStrans_clone
#include "stc/arc.h"

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
