/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_UA_H
#define C_MINILIB_SIP_UA_INT_UA_H
/*
  This is interface to int_ua module, if you need anything from
  int_ua module, put interface declaration here and interface
  implementation in .c but always write real implementation as static inline in
  _internal.
 */

#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "sip/sip.h"
#include "socket/socket.h"

/******************************************************************************
 *                             User Agent                                     *
 ******************************************************************************/
struct cmsu_Ua {
  sip_session_t sipsess;
};

static inline cme_error_t cmsu_Ua_create(evl_t evl, struct cmsu_Ua **out) {
  cme_error_t err;
  struct cmsu_Ua *ua = calloc(1, sizeof(struct cmsu_Ua));
  if (!ua) {
    err = cme_error(ENOMEM, "Cannot allocate memory for ua");
    goto error_out;
  }

  err = sip_session_create(evl, (ip_addr_t){.ip = "0.0.0.0", .port = "7337"},
                           &ua->sipsess);
  if (err) {
    goto error_ua_cleanup;
  }

  *out = ua;

  return 0;

error_ua_cleanup:
  free(ua);
error_out:
  return cme_return(err);
}

static inline void cmsu_Ua_destroy(struct cmsu_Ua **out) {
  if (!out || !*out) {
    return;
  }

  sip_session_destroy(&(*out)->sipsess);
  free(*out);
  *out = NULL;
}

#endif // C_MINILIB_SIP_UA_INT_UA_H
