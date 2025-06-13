/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIP_STRANS_HASHMAP_H
#define C_MINILIB_SIP_UA_INT_SIP_STRANS_HASHMAP_H
#include <stdint.h>

#include "c_minilib_error.h"
#include "sip_core/sip_core.h"
#include "stc/cstr.h"

/* #define i_type hmap_cmsu_SipStransMap, cstr, struct cmsu_SipServerTransaction
 */
#define i_tag cmsu_SipStransMap
#define i_keypro cstr
#define i_val sip_strans_t
#include "stc/hmap.h"

static inline cme_error_t my_hmap_cmsu_SipStransMap_insert(
    const char *key, uint32_t key_len, sip_strans_t strans,
    struct hmap_cmsu_SipStransMap *stmap, sip_strans_t *out) {

  void *result =
      hmap_cmsu_SipStransMap_insert_or_assign(
          stmap, cstr_from_sv((csview){.buf = key, .size = key_len}), strans)
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

static inline sip_strans_t
my_hmap_cmsu_SipStransMap_find(const char *key, uint32_t key_len,
                               struct hmap_cmsu_SipStransMap *stmap) {
  cstr tmp_key = cstr_from_sv((csview){.buf = key, .size = key_len});
  const char *tmp_key_2 = cstr_toraw(&tmp_key);

  if (!hmap_cmsu_SipStransMap_contains(stmap, tmp_key_2)) {
    return NULL;
  }

  return *hmap_cmsu_SipStransMap_at_mut(stmap, tmp_key_2);
}

#endif
