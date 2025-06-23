#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "event_loop/_internal/fd_helper.h"
#include "unity.h"
#include "unity_wrapper.h"

#include "event_loop/event_loop.h"
#include "my_struct.h"
#include "utils/generic_ptr.h"
#include "utils/memory.h"

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

  err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);
}

void test_EventLoopPtr_insert_sockfd_success(void) {
  cme_error_t err;

  err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  struct __MyStruct *my_struct = my_malloc(sizeof(struct __MyStruct));
  *my_struct = (struct __MyStruct){.bar = -19, .foo = 19};
  struct MyStructPtr my_struct_ptr = MyStructPtr_from_ptr(my_struct);
  gptr = GenericPtr_from_arc(MyStructPtr, &my_struct_ptr);

  err = EventLoopPtr_insert_socketfd(evlp, 10, event_loop_recvh_mock, gptr);
  MYTEST_ASSERT_ERR_NULL(err);

  TEST_ASSERT_EQUAL_INT(10, evlp.get->fds.data[0].fd);

  struct __FdHelper helper =
      hmap__FdHelpersMap_get(&evlp.get->fds_helpers, 10)->second;
  TEST_ASSERT_EQUAL_PTR(my_struct, helper.data.get);

  struct GenericPtr gptr = helper.data;
  my_struct_ptr = GenericPtr_dump(MyStructPtr, gptr);
  TEST_ASSERT_EQUAL_INT(-19, my_struct_ptr.get->bar);
  TEST_ASSERT_EQUAL_INT(19, my_struct_ptr.get->foo);
}

static cme_error_t event_loop_recvh_mock(struct GenericPtr data) {
  event_loop_recvh_i++;
  return 0;
}
