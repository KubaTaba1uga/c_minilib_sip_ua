/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UTILS_IP_H
#define C_MINILIB_SIP_UA_UTILS_IP_H
#include <inttypes.h>
#include <stdint.h>

#include "stc/cstr.h"
#include "utils/memory.h"

struct __IpAddr {
  cstr ip;
  cstr port;
};

static inline void __IpAddr_destroy(struct __IpAddr *data) {
  puts(__func__);
  cstr_drop(&data->ip);
  cstr_drop(&data->port);
};
static inline struct __IpAddr __IpAddr_clone(struct __IpAddr data) {
  puts(__func__);
  return data;
};

#define i_type IpAddrPtr
#define i_key struct __IpAddr
#define i_keydrop __IpAddr_destroy
#define i_keyclone __IpAddr_clone
#include "stc/arc.h"

static inline struct IpAddrPtr IpAddrPtr_create(cstr ip, cstr port) {
  struct __IpAddr *out = my_malloc(sizeof(struct __IpAddr));
  *out = (struct __IpAddr){.ip = ip, .port = port};

  return IpAddrPtr_from_ptr(out);
}

#undef IpAddrPtr_from
#undef IpAddrPtr_from_ptr

#endif // C_MINILIB_SIP_UA_UTILS_IP_H
