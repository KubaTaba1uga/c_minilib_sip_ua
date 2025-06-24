// test/test_udp_socket_ptr_create_error.c

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "unity.h"
#include "unity_wrapper.h"

#include "c_minilib_error.h"
#include "event_loop/_internal/fd_helper_hmap.h"
#include "event_loop/_internal/fd_vec.h"
#include "event_loop/event_loop.h"
#include "udp_socket/udp_socket.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"
#include "utils/memory.h"

//--------------------------------------------------------------------------------
bool mock_err;
cme_error_t __UdpSocket_create_fd(struct IpAddrPtr ip_addr, int32_t *fd) {
  (void)ip_addr;
  (void)fd;

  if (mock_err) {
    return cme_error(EINVAL, "mocked failure in __UdpSocket_create_fd");
  }

  *fd = 13;
  return 0;
}
//--------------------------------------------------------------------------------

struct EventLoopPtr evlp;
struct UdpSocketPtr udp;
struct IpAddrPtr ip;

void setUp(void) {
  cme_init();

  cme_error_t err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  udp = (struct UdpSocketPtr){0};
  ip = (struct IpAddrPtr){0};

  mock_err = true;
}

void tearDown(void) {
  if (ip.get) {
    IpAddrPtr_drop(&ip);
  }
  if (udp.get) {
    UdpSocketPtr_drop(&udp);
  }
  if (evlp.get) {
    EventLoopPtr_drop(&evlp);
  }
  cme_destroy();
}

void test_UdpSocketPtr_create_fd_error_is_reported(void) {
  cme_error_t err;

  ip = IpAddrPtr_create(cstr_lit("127.0.0.1"), cstr_lit("9999"));
  TEST_ASSERT_NOT_NULL(ip.get);

  err = UdpSocketPtr_create(evlp, ip, &udp);

  // should fail
  MYTEST_ASSERT_ERR_NOT_NULL(err);
  TEST_ASSERT_NULL(udp.get);
}

void test_UdpSocketPtr_create_success_inserts_into_event_loop(void) {
  cme_error_t err;

  // Arrange: allow create_fd to succeed
  mock_err = false;

  ip = IpAddrPtr_create(cstr_lit("127.0.0.1"), cstr_lit("0"));
  TEST_ASSERT_NOT_NULL(ip.get);

  // Act
  err = UdpSocketPtr_create(evlp, ip, &udp);
  MYTEST_ASSERT_ERR_NULL(err);

  // Assert: we got a valid UdpSocketPtr
  TEST_ASSERT_NOT_NULL(udp.get);
  TEST_ASSERT_EQUAL_INT(13, udp.get->fd);

  // The FD should be in the event loop's poll vector
  TEST_ASSERT_EQUAL_INT(13, evlp.get->fds.data[0].fd);

  // And a helper entry must exist
  struct __FdHelper *helper = __FdHelpersMap_find(13, &evlp.get->fds_helpers);
  TEST_ASSERT_NOT_NULL(helper);

  // The helper data must point back to our udp ptr
  struct UdpSocketPtr udp_from_helper =
      GenericPtr_dump(UdpSocketPtr, helper->data);
  TEST_ASSERT_EQUAL_PTR(udp.get, udp_from_helper.get);
  TEST_ASSERT_EQUAL_INT(*udp.use_count, *udp_from_helper.use_count);
}
