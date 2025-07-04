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
#include "common.h"
#include "event_loop/event_loop.h"
#include "sip_core/_internal/sip_core.h"
#include "sip_core/_internal/sip_server_transaction/_internal/sip_server_transaction_invite.h"
#include "sip_core/_internal/sip_server_transaction/sip_server_transaction.h"
#include "sip_core/sip_core.h"
#include "sip_transport/_internal/sip_transport.h"
#include "stc/cstr.h"
#include "unity.h"
#include "unity_wrapper.h"
#include "utils/ip.h"

static struct SipServerTransactionPtr strans;
static struct SipMessagePtr sipmsg;
static struct IpAddrPtr ipaddr;
static struct EventLoopPtr evl;
static struct SipCorePtr sip_core;
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
  strans = (struct SipServerTransactionPtr){0};
  sipmsg = (struct SipMessagePtr){0};
  evl = (struct EventLoopPtr){0};
  sip_core = (struct SipCorePtr){0};
  ipaddr = (struct IpAddrPtr){0};

  _status_code = 0;
  _status_phrase = (cstr){0};
}

void tearDown(void) {
  if (sipmsg.get) {
    SipMessagePtr_drop(&sipmsg);
  }

  if (ipaddr.get) {
    IpAddrPtr_drop(&ipaddr);
  }

  if (sip_core.get) {
    SipCorePtr_drop(&sip_core);
  }

  if (evl.get) {
    EventLoopPtr_drop(&evl);
  }

  cme_destroy();
}

/* Test plan:
    1. Mock `__SipServerTransactionPtr_invite_reply` to do not send msg. - DONE
    2. Add server transaction to sip core (this simulates rettransmission).
    3. Run `__SipServerTransactionPtr_invite_recv` with another INVITE msg.
    4. Check if 100 and TRYING was replied in mock.
 */
void test_sip_strans_invite_recv_retransmission_success(void) {
  cme_error_t err;

  ipaddr = IpAddrPtr_create(cstr_from("127.0.0.1"), cstr_from("1234"));
  err = EventLoopPtr_create(&evl);
  MYTEST_ASSERT_ERR_NULL(err);

  err = SipCorePtr_create(evl, ipaddr, SipTransportProtocolType_UDP, &sip_core);
  MYTEST_ASSERT_ERR_NULL(err);

  sipmsg = create_dummy_sipmsg();

  err = __SipServerTransactionPtr_create(sipmsg, sip_core, ipaddr, &strans);
  MYTEST_ASSERT_ERR_NULL(err);

  err = __SipServerTransactionPtr_invite_recv(sipmsg, ipaddr, &strans);
  MYTEST_ASSERT_ERR_NULL(err);

  TEST_ASSERT_EQUAL(100, _status_code);
  TEST_ASSERT_EQUAL_STRING("TRYING", cstr_data(&_status_phrase));
}
