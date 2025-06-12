/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_STRANS_HASHMAP_H
#define C_MINILIB_SIP_UA_INT_SIP_STRANS_HASHMAP_H
#include <stdint.h>

#include "c_minilib_error.h"
#include "sip_core/_internal/sip_strans.h"
#include "sip_core/sip_core.h"
#include "stc/cstr.h"

/* #define i_type hmap_cmsu_SipStransMap, cstr, struct cmsu_SipServerTransaction
 */
#define i_tag cmsu_SipStransMap
#define i_keypro cstr
#define i_val struct cmsu_SipServerTransaction
#include "stc/hmap.h"

static inline cme_error_t my_hmap_cmsu_SipStransMap_insert_new(
    const char *key, uint32_t key_len, sip_msg_t sip_msg, sip_core_t sip_core,
    struct hmap_cmsu_SipStransMap *stmap, sip_strans_t *out) {

  void *result =
      hmap_cmsu_SipStransMap_insert_or_assign(
          stmap, cstr_from_sv((csview){.buf = key, .size = key_len}),
          (struct cmsu_SipServerTransaction){
              .core = sip_core, .state = cmsu_SipStransState_PROCEEDING})
          .ref;
  cme_error_t err;

  if (!result) {
    err = cme_error(ENOMEM, "Cannot insert new server transaction to sip core");
    goto error_out;
  }

  *out = result;

  return 0;

error_out:
  return cme_return(err);
}

#endif
