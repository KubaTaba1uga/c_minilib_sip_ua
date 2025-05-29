/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_ERROR_H
#define C_MINILIB_SIP_UA_ERROR_H

#include "c_minilib_error.h"

#define CMSU_CHECK_ERR(err)                                                    \
  if (err) {                                                                   \
    return cme_return(err);                                                    \
  }

#endif
