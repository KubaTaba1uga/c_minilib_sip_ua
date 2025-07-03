/*
 * Tests for __FdHelpersMap helpers
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <stdint.h>
#include <string.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/fd_helper.h"
#include "event_loop/_internal/fd_helper_hmap.h"
#include "unity.h"
#include "unity_wrapper.h"

static int recv_counter;
static int timeout_counter;

static cme_error_t dummy_recvh(struct GenericPtr data) {
  (void)data;
  recv_counter++;
  return 0;
}
static cme_error_t dummy_timeouth(struct GenericPtr data) {
  (void)data;
  timeout_counter++;
  return 0;
}

static inline __FdHelper make_helper(void) {
  return (__FdHelper){.fd_type = __FdType_SOCKET,
                      .recvh = dummy_recvh,
                      .timeouth = dummy_timeouth,
                      .data = (struct GenericPtr){0}};
}

static __FdHelpersMap helpers;

void setUp(void) {
  cme_init();
  helpers = hmap__FdHelpersMap_init();
  recv_counter = 0;
  timeout_counter = 0;
}

void tearDown(void) {
  hmap__FdHelpersMap_drop(&helpers);
  cme_destroy();
}

void test_FdHelpersMap_insert_find_and_callbacks(void) {
  cme_error_t err = __FdHelpersMap_insert(42, make_helper(), &helpers);
  MYTEST_ASSERT_ERR_NULL(err);

  __FdHelper *found = __FdHelpersMap_find(42, &helpers);
  TEST_ASSERT_NOT_NULL(found);
  TEST_ASSERT_EQUAL_INT(__FdType_SOCKET, found->fd_type);

  /* Invoke callbacks and make sure counters bump */
  found->recvh(found->data);
  found->timeouth(found->data);
  TEST_ASSERT_EQUAL_INT(1, recv_counter);
  TEST_ASSERT_EQUAL_INT(1, timeout_counter);
}

void test_FdHelpersMap_remove(void) {
  __FdHelpersMap_insert(7, make_helper(), &helpers);
  __FdHelpersMap_remove(7, &helpers);
  TEST_ASSERT_NULL(__FdHelpersMap_find(7, &helpers));
}

void test_FdHelpersMap_find_missing(void) {
  TEST_ASSERT_NULL(__FdHelpersMap_find(99, &helpers));
}
