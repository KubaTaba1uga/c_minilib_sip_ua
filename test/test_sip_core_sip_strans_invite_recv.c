/*
 * Tests for __FdHelpersMap helpers
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "c_minilib_error.h"
#include "sip_core/_internal/sip_server_transaction/_internal/sip_server_transaction_invite.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "unity.h"
#include "unity_wrapper.h"

static uint32_t _status_code;
static cstr _status_phrase;

cme_error_t ____SipServerTransactionPtr_invite_reply(
    uint32_t status_code, cstr status_phrase,
    struct SipServerTransactionPtr strans) {
  _status_code = status_code;
  _status_phrase = status_phrase;
  puts("MOCKED");
  return 0;
};

void setUp(void) {
  cme_init();

  _status_code = 0;
  _status_phrase = (cstr){0};
}

void tearDown(void) { cme_destroy(); }

/* Test plan:
    1. Mock `__SipServerTransactionPtr_invite_reply` to do not send msg. - DONE
    2. Add server transaction to sip core (this simulates rettransmission).
    3. Run `__SipServerTransactionPtr_invite_recv` with another INVITE msg.
    4. Check if 100 and TRYING was replied in mock.
 */
void test_sip_strans_invite_recv_retransmission_success(void) {

  /* __SipServerTransactionPtr_invite_recv(struct SipMessagePtr sipmsg, struct
   * IpAddrPtr peer_ip, struct SipServerTransactionPtr *strans) */
}
