/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SIP_SERVER_TRANSACTION_H
#define C_MINILIB_SIP_UA_SIP_SERVER_TRANSACTION_H
#include <stdint.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "event_loop/event_loop.h"
#include "sip_transport/sip_transport.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"

#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_server_transaction/_internal/sip_server_transaction.h"

/******************************************************************************
 *                          Sip Server Transaction                            *
 ******************************************************************************/
cme_error_t
SipServerTransactionPtr_reply(uint32_t status_code, cstr status_phrase,
                              struct SipServerTransactionPtr *sip_strans);

/*
  If TU wants to be informed about transaction errors, TU need to set up
  error handling.
*/
void SipServerTransactionPtr_set_errh(sip_core_strans_errh_t errh,
                                      struct GenericPtr arg,
                                      struct SipServerTransactionPtr strans);

#endif // C_MINILIB_SIP_UA_SIP_SERVER_TRANSACTION_H
