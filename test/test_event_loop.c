#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/fd_helper.h"
#include "event_loop/_internal/fd_helper_hmap.h"
#include "event_loop/_internal/fd_vec.h"
#include "stc/algorithm.h"
#include "stc/common.h"
#include "unity.h"
#include "unity_wrapper.h"

#include "event_loop/event_loop.h"
#include "my_struct.h"
#include "utils/generic_ptr.h"
#include "utils/memory.h"
#include "utils/stc_wrapper.h"

static cme_error_t event_loop_recvh_mock(struct GenericPtr data);

struct GenericPtr gptr;
struct EventLoopPtr evlp;
uint32_t event_loop_recvh_i;

void setUp(void) {
  cme_init();
  gptr = (struct GenericPtr){0};
  evlp = (struct EventLoopPtr){0};
  event_loop_recvh_i = 0;
}

void tearDown(void) {
  if (evlp.get) {
    EventLoopPtr_drop(&evlp);
  }

  if (gptr.get) {
    struct MyStructPtr my_struct_ptr = GenericPtr_dump(MyStructPtr, gptr);
    MyStructPtr_drop(&my_struct_ptr);
  }

  cme_destroy();
}

void test_EventLoopPtr_create_success(void) {
  cme_error_t err;

  TEST_ASSERT_NULL(evlp.get);

  err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  TEST_ASSERT_NOT_NULL(evlp.get);
}

void test_EventLoopPtr_insert_sockfd_success(void) {
  cme_error_t err;

  err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  struct __MyStruct *my_struct = my_malloc(sizeof(struct __MyStruct));
  *my_struct = (struct __MyStruct){.bar = -19, .foo = 19};
  struct MyStructPtr my_struct_ptr = MyStructPtr_from_ptr(my_struct);
  gptr = GenericPtr_from_arc(MyStructPtr, my_struct_ptr);

  err = EventLoopPtr_insert_socketfd(evlp, 10, event_loop_recvh_mock, gptr);
  MYTEST_ASSERT_ERR_NULL(err);

  TEST_ASSERT_EQUAL_INT(10, evlp.get->fds.data[0].fd);

  struct __FdHelper *helper = __FdHelpersMap_find(10, &evlp.get->fds_helpers);
  TEST_ASSERT_NOT_NULL(helper);
  TEST_ASSERT_EQUAL_PTR(my_struct, helper->data.get);
  TEST_ASSERT_EQUAL_PTR(event_loop_recvh_mock, helper->recvh);
  TEST_ASSERT_NULL(helper->timeouth);

  struct GenericPtr gptr = helper->data;
  my_struct_ptr = GenericPtr_dump(MyStructPtr, gptr);
  TEST_ASSERT_EQUAL_INT(-19, my_struct_ptr.get->bar);
  TEST_ASSERT_EQUAL_INT(19, my_struct_ptr.get->foo);
}

void test_EventLoopPtr_insert_timerfd_success(void) {
  cme_error_t err;

  err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  struct __MyStruct *my_struct = my_malloc(sizeof(struct __MyStruct));
  *my_struct = (struct __MyStruct){.bar = 123, .foo = 456};
  struct MyStructPtr my_struct_ptr = MyStructPtr_from_ptr(my_struct);
  gptr = GenericPtr_from_arc(MyStructPtr, my_struct_ptr);

  err = EventLoopPtr_insert_timerfd(evlp, 12, event_loop_recvh_mock, gptr);
  MYTEST_ASSERT_ERR_NULL(err);

  TEST_ASSERT_EQUAL_INT(12, evlp.get->fds.data[0].fd);

  struct __FdHelper helper =
      hmap__FdHelpersMap_get(&evlp.get->fds_helpers, 12)->second;
  TEST_ASSERT_EQUAL_PTR(my_struct, helper.data.get);
  TEST_ASSERT_EQUAL_PTR(event_loop_recvh_mock, helper.timeouth);
  TEST_ASSERT_NULL(helper.recvh);

  struct GenericPtr gptr = helper.data;
  my_struct_ptr = GenericPtr_dump(MyStructPtr, gptr);
  TEST_ASSERT_EQUAL_INT(123, my_struct_ptr.get->bar);
  TEST_ASSERT_EQUAL_INT(456, my_struct_ptr.get->foo);
}

static cme_error_t event_loop_recvh_mock(struct GenericPtr data) {
  event_loop_recvh_i++;
  return 0;
}

void test_EventLoopPtr_remove_fd_success(void) {
  cme_error_t err;

  err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  struct __MyStruct *my_struct = my_malloc(sizeof(struct __MyStruct));
  *my_struct = (struct __MyStruct){.bar = 111, .foo = 222};
  struct MyStructPtr my_struct_ptr = MyStructPtr_from_ptr(my_struct);
  gptr = GenericPtr_from_arc(MyStructPtr, my_struct_ptr);

  err = EventLoopPtr_insert_timerfd(evlp, 7, event_loop_recvh_mock, gptr);
  MYTEST_ASSERT_ERR_NULL(err);

  // Ensure FD is present
  __PollFd *fd_ptr = __PollFdsVec_find(7, &evlp.get->fds);
  TEST_ASSERT_NOT_NULL(fd_ptr);
  __FdHelper *helper_ptr = __FdHelpersMap_find(7, &evlp.get->fds_helpers);
  TEST_ASSERT_NOT_NULL(helper_ptr);

  // Now remove
  EventLoopPtr_remove_fd(evlp, 7);

  // Confirm FD is removed from poll vector
  fd_ptr = __PollFdsVec_find(7, &evlp.get->fds);
  TEST_ASSERT_NULL(fd_ptr);

  // Confirm helper is removed
  helper_ptr = __FdHelpersMap_find(7, &evlp.get->fds_helpers);
  TEST_ASSERT_NULL(helper_ptr);
}

void test_EventLoopPtr_set_pollin_success(void) {
  cme_error_t err;

  err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  struct __MyStruct *my_struct = my_malloc(sizeof(*my_struct));
  *my_struct = (struct __MyStruct){.bar = 314, .foo = 159};
  struct MyStructPtr my_struct_ptr = MyStructPtr_from_ptr(my_struct);
  gptr = GenericPtr_from_arc(MyStructPtr, my_struct_ptr);

  err = EventLoopPtr_insert_socketfd(evlp, 30, event_loop_recvh_mock, gptr);
  MYTEST_ASSERT_ERR_NULL(err);

  // Before set_pollin, events field should be zero
  __PollFd *slot = __PollFdsVec_find(30, &evlp.get->fds);
  TEST_ASSERT_NOT_NULL(slot);
  TEST_ASSERT_EQUAL_INT(0, slot->events);

  err = EventLoopPtr_set_pollin(evlp, 30);
  MYTEST_ASSERT_ERR_NULL(err);

  slot = __PollFdsVec_find(30, &evlp.get->fds);
  TEST_ASSERT_NOT_NULL(slot);
  TEST_ASSERT_TRUE(slot->events & POLLIN);

  EventLoopPtr_remove_fd(evlp, 30);
}

void test_EventLoopPtr_set_pollin_fd_not_found(void) {
  cme_error_t err;

  err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  err = EventLoopPtr_set_pollin(evlp, 99);
  TEST_ASSERT_NOT_NULL(err);
}
