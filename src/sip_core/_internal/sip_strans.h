/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_STRANS_H
#define C_MINILIB_SIP_UA_INT_SIP_STRANS_H

#include "sip_core/sip_core.h"

enum cmsu_SipStransState {
  cmsu_SipStransState_TRYING,
  cmsu_SipStransState_PROCEEDING,
  cmsu_SipStransState_ACCEPTED,
  cmsu_SipStransState_COMPLETED,
  cmsu_SipStransState_CONFIRMED,
};

struct cmsu_SipServerTransaction {
  sip_core_t core;
  enum cmsu_SipStransState state;
};

static inline cme_error_t
cmsu_SipStrans_init(sip_msg_t sip_msg, sip_core_t sip_core,
                    struct cmsu_SipServerTransaction *out) {
  return 0;
};

void cmsu_SipStrans_destroy(struct cmsu_SipServerTransaction **out);

#endif
