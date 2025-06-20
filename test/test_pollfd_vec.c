#include <poll.h> // for POLLIN, POLLOUT
#include <stdio.h>
#include <unity.h>

#include "event_loop/_internal/poll_fd_vec.h"

static int drop_call_count = 0;

// Mock drop implementation to verify call count
void cmsu_PollFd_drop(cmsu_PollFd *self) {
  (void)self;
  drop_call_count++;
}

void setUp(void) { drop_call_count = 0; }

void tearDown(void) {
  // No cleanup needed
}

void test_push_and_sort(void) {
  cmsu_PollFds vec = {0};

  cmsu_PollFd a = {.fd = 5, .events = POLLIN, .revents = 0};
  cmsu_PollFd b = {.fd = 1, .events = POLLOUT, .revents = 0};
  cmsu_PollFd c = {.fd = 3, .events = POLLIN, .revents = 0};

  vec_cmsu_PollFds_push(&vec, cmsu_PollFd_clone(a));
  vec_cmsu_PollFds_push(&vec, cmsu_PollFd_clone(b));
  vec_cmsu_PollFds_push(&vec, cmsu_PollFd_clone(c));

  vec_cmsu_PollFds_sort(&vec);

  TEST_ASSERT_EQUAL_INT(1, vec.data[0].fd);
  TEST_ASSERT_EQUAL_INT(3, vec.data[1].fd);
  TEST_ASSERT_EQUAL_INT(5, vec.data[2].fd);

  vec_cmsu_PollFds_drop(&vec);
  TEST_ASSERT_EQUAL_INT(3, drop_call_count);
}

void test_drop_calls_drop_function(void) {
  cmsu_PollFds vec = {0};

  cmsu_PollFd a = {.fd = 10, .events = POLLIN, .revents = 0};
  cmsu_PollFd b = {.fd = 11, .events = POLLOUT, .revents = 0};

  vec_cmsu_PollFds_push(&vec, cmsu_PollFd_clone(a));
  vec_cmsu_PollFds_push(&vec, cmsu_PollFd_clone(b));

  vec_cmsu_PollFds_drop(&vec);

  TEST_ASSERT_EQUAL_INT(2, drop_call_count);
}
