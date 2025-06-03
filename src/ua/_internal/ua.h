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
#include "c_minilib_sip_codec.h"
#include "sip/sip.h"
#include "socket/socket.h"

/******************************************************************************
 *                             User Agent                                     *
 ******************************************************************************/
struct cmsu_Ua {
  sip_stack_t sipstack;
};

static inline cme_error_t cmsu_Ua_create(evl_t evl, struct cmsu_Ua **out) {
  cme_error_t err;
  struct cmsu_Ua *ua = calloc(1, sizeof(struct cmsu_Ua));
  if (!ua) {
    err = cme_error(ENOMEM, "Cannot allocate memory for ua");
    goto error_out;
  }

  err = sip_stack_create(evl, (ip_addr_t){.ip = "0.0.0.0", .port = "7337"},
                         &ua->sipstack);
  if (err) {
    goto error_ua_cleanup;
  }

  sip_msg_t msg;
  err = cmsc_sipmsg_create_with_buf(&msg);
  if (err) {
    goto error_ua_cleanup;
  }

  err = cmsc_sipmsg_insert_request_line(
      strlen("SIP/2.0"), "SIP/2.0", strlen("sip:bob@example.com"),
      "sip:bob@example.com", strlen("MESSAGE"), "MESSAGE", msg);
  if (err) {
    goto error_ua_cleanup;
  }

  err = cmsc_sipmsg_insert_from(strlen("<sip:alice@example.com>"),
                                "<sip:alice@example.com>", strlen("a1b2c3"),
                                "a1b2c3", msg);
  if (err) {
    goto error_ua_cleanup;
  }

  err = cmsc_sipmsg_insert_to(strlen("<sip:bob@example.com>"),
                              "<sip:bob@example.com>", strlen("x9y8z7"),
                              "x9y8z7", msg);
  if (err) {
    goto error_ua_cleanup;
  }

  err = cmsc_sipmsg_insert_call_id(strlen("call-1234"), "call-1234", msg);
  if (err) {
    goto error_ua_cleanup;
  }

  err = cmsc_sipmsg_insert_cseq(strlen("MESSAGE"), "MESSAGE", 42, msg);
  if (err) {
    goto error_ua_cleanup;
  }

  err = cmsc_sipmsg_insert_via(
      strlen("SIP/2.0/UDP"), "SIP/2.0/UDP", strlen("client.example.com"),
      "client.example.com", 0, NULL, strlen("z9hG4bKbranch123"),
      "z9hG4bKbranch123", 0, NULL, 0, msg);
  if (err) {
    goto error_ua_cleanup;
  }

  err = sip_stack_connect(ua->sipstack,
                          (ip_addr_t){.ip = "127.0.0.1", .port = "8888"}, msg);
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

  sip_stack_destroy(&(*out)->sipstack);
  free(*out);
  *out = NULL;
}

#endif // C_MINILIB_SIP_UA_INT_UA_H
