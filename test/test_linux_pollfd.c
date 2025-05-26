#include "event_loop/linux_poll.h"
#include <unity.h>

void test_push_and_sort(void) {
  struct vec_cmsu_PollFds vec = {0};

  cmsu_PollFd a = {.fd = 3, .events = POLLIN, .revents = 0};
  cmsu_PollFd b = {.fd = 1, .events = POLLOUT, .revents = 0};
  cmsu_PollFd c = {.fd = 5, .events = POLLIN, .revents = 0};

  vec_cmsu_PollFds_push(&vec, cmsu_PollFd_clone(c));
  vec_cmsu_PollFds_push(&vec, cmsu_PollFd_clone(a));
  vec_cmsu_PollFds_push(&vec, cmsu_PollFd_clone(b));

  // Sort using the default cmp (a->fd > b->fd)
  vec_cmsu_PollFds_sort(&vec);

  TEST_ASSERT_EQUAL_INT(1, vec.data[0].fd);
  TEST_ASSERT_EQUAL_INT(3, vec.data[1].fd);
  TEST_ASSERT_EQUAL_INT(5, vec.data[2].fd);

  vec_cmsu_PollFds_drop(&vec);
}

/* void test_drop_closes_fds(void) { */
/*   struct cmsu_PollFds vec = vec_cmsu_PollFds_init(); */

/*   int pipe_fds[2]; */
/*   pipe(pipe_fds); // pipe_fds[0]: read, pipe_fds[1]: write */

/*   cmsu_PollFd pfd = {.fd = pipe_fds[0], .events = POLLIN, .revents = 0}; */
/*   cmsu_PollFds_push(&vec, cmsu_pollfd_clone(pfd)); */

/*   // Drop will call close on pipe_fds[0] */
/*   cmsu_PollFds_drop(&vec); */

/*   // Now read from closed FD should fail */
/*   char buf; */
/*   ssize_t res = read(pipe_fds[0], &buf, 1); */
/*   TEST_ASSERT_EQUAL_INT(-1, res); */
/* } */
