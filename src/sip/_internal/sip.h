/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_INT_SIPSESS_H
#define C_MINILIB_SIP_UA_INT_SIPSESS_H
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip/sip.h"
#include "socket/socket.h"

/******************************************************************************
 *                             Sip Session                                    *
 ******************************************************************************/
struct cmsu_SipSession {
  socket_t socket;
};

static inline void cmsu_SipSession_destroy(void *out) {
  if (!out) {
    return;
  }

  struct cmsu_SipSession **sipsess = out;
  if (!*sipsess) {
    return;
  }

  socket_destroy(&(*sipsess)->socket);
  free(*sipsess);
  *sipsess = NULL;
}

static inline cme_error_t cmsu_SipSession_recv_callback(socket_t, buffer_t *,
                                                        void *);
static inline cme_error_t cmsu_SipSession_send_callback(socket_t, ip_addr_t,
                                                        buffer_t *, void *);

static inline cme_error_t cmsu_SipSession_create(cmsu_evl_t evl,
                                                 ip_addr_t ipaddr,
                                                 struct cmsu_SipSession **out) {
  struct cmsu_SipSession *sipsess = calloc(1, sizeof(struct cmsu_SipSession));
  cme_error_t err;
  if (!sipsess) {
    err = cme_error(ENOMEM, "Cannot allocate memory for ua");
    goto error_out;
  }

  err = socket_udp_create(evl, ipaddr, cmsu_SipSession_recv_callback,
                          cmsu_SipSession_send_callback,
                          cmsu_SipSession_destroy, sipsess, &sipsess->socket);
  if (err) {
    goto error_sipsess_cleanup;
  }

  err = event_loop_insert_socket(sipsess->socket, evl);
  if (err) {
    goto error_udp_cleanup;
  }

  *out = sipsess;

  return 0;

error_udp_cleanup:
  socket_destroy(&sipsess->socket);
error_sipsess_cleanup:
  free(sipsess);
error_out:
  return cme_return(err);
}

static inline cme_error_t
cmsu_SipSession_recv_callback(socket_t socket, buffer_t *buf, void *ctx) {
  puts("Hit sip recv");
  return 0;
}

static inline cme_error_t cmsu_SipSession_send_callback(socket_t socket,
                                                        ip_addr_t recv,
                                                        buffer_t *buf,
                                                        void *ctx) {
  puts("Hit sip send");
  return 0;
};

#endif
