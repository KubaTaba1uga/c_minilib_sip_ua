/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_MAP_H
#define C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_MAP_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sip_core/_internal/common.h"
#include "sip_core/sip_core.h"
#include "utils/sip_msg.h"

static inline cme_error_t __SipCoreStransMap_insert(csview key,
                                                    sip_strans_t strans,
                                                    __SipCoreStransMap *stmap,
                                                    sip_strans_t *out) {

  sip_strans_t result =
      hmap__SipCoreStransMap_insert_or_assign(stmap, cstr_from_sv(key), strans)
          .ref->second;
  cme_error_t err;

  if (!result) {
    err = cme_error(ENOMEM, "Cannot insert new server transaction to sip core");
    goto error_out;
  }

  *out = result;

  return 0;

error_out:
  __SipCoreStrans_deref(strans);
  *out = NULL;
  return cme_return(err);
}

static inline sip_strans_t __SipCoreStransMap_find(csview key,
                                                   __SipCoreStransMap *stmap) {
  cstr cstr_key = cstr_from_sv(key);
  const char *raw_key = cstr_toraw(&cstr_key);

  if (!hmap__SipCoreStransMap_contains(stmap, raw_key)) {
    return NULL;
  }

  return *hmap__SipCoreStransMap_at_mut(stmap, raw_key);
}

#endif // C_MINILIB_SIP_UA_INT_SIP_CORE_STRANS_MAP_H
