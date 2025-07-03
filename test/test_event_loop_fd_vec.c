/*
 * Tests for __PollFdsVec helpers
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <poll.h>
#include <stdint.h>
#include <string.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/fd_vec.h"
#include "unity.h"
#include "unity_wrapper.h"

static vec__PollFdsVec fds;

void setUp(void) {
  cme_init();
  fds = vec__PollFdsVec_init();
}

void tearDown(void) {
  vec__PollFdsVec_drop(&fds);
  cme_destroy();
}

void test_PollFdsVec_push_and_find(void) {
  cme_error_t err = __PollFdsVec_push(&fds, (__PollFd){.fd = 10});
  MYTEST_ASSERT_ERR_NULL(err);
  TEST_ASSERT_EQUAL_UINT32(1, fds.size);
  TEST_ASSERT_EQUAL_INT(10, fds.data[0].fd);
  __PollFd *found = __PollFdsVec_find(10, &fds);
  TEST_ASSERT_NOT_NULL(found);
  TEST_ASSERT_EQUAL_INT(10, found->fd);
}

void test_PollFdsVec_set_and_unset_events(void) {
  cme_error_t err = __PollFdsVec_push(&fds, (__PollFd){.fd = 11});
  MYTEST_ASSERT_ERR_NULL(err);

  /* set POLLIN */
  err = __PollFdsVec_set_pollin(11, &fds);
  MYTEST_ASSERT_ERR_NULL(err);
  TEST_ASSERT_TRUE(fds.data[0].events & POLLIN);

  /* set POLLOUT */
  err = __PollFdsVec_set_pollout(11, &fds);
  MYTEST_ASSERT_ERR_NULL(err);
  TEST_ASSERT_TRUE(fds.data[0].events & POLLIN);
  TEST_ASSERT_TRUE(fds.data[0].events & POLLOUT);

  /* unset POLLOUT */
  err = __PollFdsVec_unset_pollout(11, &fds);
  MYTEST_ASSERT_ERR_NULL(err);
  TEST_ASSERT_TRUE(fds.data[0].events & POLLIN);
  TEST_ASSERT_FALSE(fds.data[0].events & POLLOUT);
}

void test_PollFdsVec_remove(void) {
  cme_error_t err = __PollFdsVec_push(&fds, (__PollFd){.fd = 12});
  MYTEST_ASSERT_ERR_NULL(err);
  __PollFdsVec_remove(12, &fds);
  TEST_ASSERT_EQUAL_UINT32(0, fds.size);
  TEST_ASSERT_NULL(__PollFdsVec_find(12, &fds));
}

void test_PollFdsVec_missing_fd_errors(void) {
  /* Attempt to modify non-existent entry should yield error object */
  cme_error_t err = __PollFdsVec_set_pollout(99, &fds);
  TEST_ASSERT_NOT_NULL(err);
}
