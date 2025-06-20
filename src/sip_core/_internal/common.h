/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
#include <stdint.h>
#include <stdio.h>

#include "c_minilib_error.h"
#include "sip_core/sip_core.h"
#include "timer_fd/timer_fd.h"
#include "utils/sip_msg.h"

struct __SipCoreListener {
  sip_core_request_handler_t request_handler;
  void *arg;
};

#define i_tag _SipCoreListenersQueue
#define i_key struct __SipCoreListener
#include "stc/queue.h"

typedef struct queue__SipCoreListenersQueue __SipCoreListenersQueue;

enum __SipCoreStransState {
  __SipCoreStransState_TRYING,     // Initial state before anything is sent
  __SipCoreStransState_PROCEEDING, // After sending 100 Trying or 1xx
                                   // provisional
  __SipCoreStransState_COMPLETED,  // After sending 3xx–6xx final response
  __SipCoreStransState_CONFIRMED,  // After receiving ACK for 3xx–6xx response
  __SipCoreStransState_TERMINATED, // After sending 2xx final response or
                                   // completing ACK processing

};

struct __SipCoreStrans {
  enum __SipCoreStransState state;
  timer_fd_t invite_100_timer;
  sip_core_t sip_core;
  sip_msg_t request;
  bool is_invite;
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

static inline sip_strans_t __SipCoreStrans_ref(sip_strans_t sip_stransp) {
  if (!sip_stransp) {
    return NULL;
  }

  __SipCoreStransPtr_clone(*sip_stransp);

  return sip_stransp;
}

static inline void __SipCoreStrans_deref(sip_strans_t sip_stransp) {
  if (!sip_stransp) {
    return;
  }

  int32_t usage_count = __SipCoreStransPtr_use_count(sip_stransp);

  __SipCoreStransPtr_drop(sip_stransp);

  // If usage count is 1 before drop it means it will be 0
  //  after drop but ptr holding usage count get freed on drop.
  if (usage_count <= 1) {
    free(sip_stransp);
  }
}

static inline void __sip_strans_destroy(sip_strans_t *data) {
  printf("%ld\n", *(*data)->use_count);
  __SipCoreStrans_deref(*data);
};

static inline sip_strans_t __sip_strans_clone(sip_strans_t data) {
  return data;
};

#define i_tag _SipCoreStransMap
#define i_keypro cstr
#define i_val sip_strans_t
#define i_valdrop __sip_strans_destroy
#define i_valclone __sip_strans_clone
#include "stc/hmap.h"

typedef struct hmap__SipCoreStransMap __SipCoreStransMap;

struct __SipCore {
  // Transp Protocol data
  event_loop_t evl;
  sip_transp_t sip_transp;

  // SIP core data
  __SipCoreListenersQueue listeners;
  __SipCoreStransMap stranses;
};

static inline void __SipCore_destroy(void *data) {
  struct __SipCore *sip_core = data;

  queue__SipCoreListenersQueue_drop(&sip_core->listeners);
  hmap__SipCoreStransMap_drop(&sip_core->stranses);
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

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_COMMON_H
