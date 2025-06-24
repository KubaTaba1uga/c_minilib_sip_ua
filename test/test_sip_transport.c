// test/test_sip_transport.c

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "unity.h"
#include "unity_wrapper.h"

#include "c_minilib_error.h"
#include "event_loop/event_loop.h"
#include "sip_transport/sip_transport.h"
#include "utils/generic_ptr.h"
#include "utils/ip.h"

//--------------------------------------------------------------------------------
// Mock __UdpSocket_create_fd to control UdpSocketPtr_create behavior
bool __UdpSocket_create_fd_mock_err;
cme_error_t __UdpSocket_create_fd(struct IpAddrPtr ip_addr, int32_t *fd) {
  (void)ip_addr;
  if (__UdpSocket_create_fd_mock_err) {
    return cme_error(EINVAL, "mocked failure in __UdpSocket_create_fd");
  }
  *fd = 13;
  return 0;
}
//--------------------------------------------------------------------------------

static struct EventLoopPtr evlp;
static struct IpAddrPtr ip;
static struct SipTransportPtr stptr;

void setUp(void) {
  cme_init();
  cme_error_t err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);
  // don't allocate ip yet
  ip = (struct IpAddrPtr){0};
  stptr = (struct SipTransportPtr){0};
  __UdpSocket_create_fd_mock_err = true;
}

void tearDown(void) {
  if (stptr.get)
    SipTransportPtr_drop(&stptr);
  if (ip.get)
    IpAddrPtr_drop(&ip);
  if (evlp.get)
    EventLoopPtr_drop(&evlp);
  cme_destroy();
}

void test_SipTransportPtr_create_udp_fd_error(void) {
  // Arrange: mock failure in underlying UDP socket creation
  __UdpSocket_create_fd_mock_err = true;

  ip = IpAddrPtr_create(cstr_lit("127.0.0.1"), cstr_lit("7337"));
  TEST_ASSERT_NOT_NULL(ip.get);

  // Act
  cme_error_t err =
      SipTransportPtr_create(evlp, ip, SipTransportProtocolType_UDP, &stptr);

  // Assert: error is propagated, and no transport ptr out
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_NULL(stptr.get);
}

void test_SipTransportPtr_create_udp_success(void) {
  // Arrange: underlying UDP socket creation succeeds
  __UdpSocket_create_fd_mock_err = false;

  ip = IpAddrPtr_create(cstr_lit("127.0.0.1"), cstr_lit("7337"));
  TEST_ASSERT_NOT_NULL(ip.get);

  // Act
  cme_error_t err =
      SipTransportPtr_create(evlp, ip, SipTransportProtocolType_UDP, &stptr);
  MYTEST_ASSERT_ERR_NULL(err);
  TEST_ASSERT_NOT_NULL(stptr.get);

  // The proto_type field must be set
  TEST_ASSERT_EQUAL_INT(SipTransportProtocolType_UDP, stptr.get->proto_type);

  // And the cloned event loop pointer must match (same internal pointer)
  // NOTE: we compare the raw pointer, not the struct:
  TEST_ASSERT_EQUAL_PTR(evlp.get, stptr.get->evl.get);

  // Verify that the UDP socket FD from the mock (13) was stored
  TEST_ASSERT_EQUAL_INT(13, stptr.get->udp_socket.get->fd);
}

void test_SipTransportPtr_create_unsupported_proto(void) {
  // Arrange: any IP is fine, underlying mock not used
  __UdpSocket_create_fd_mock_err = false;

  ip = IpAddrPtr_create(cstr_lit("127.0.0.1"), cstr_lit("7337"));
  TEST_ASSERT_NOT_NULL(ip.get);

  // Act: use an out-of-range protocol
  cme_error_t err = SipTransportPtr_create(
      evlp, ip, (enum SipTransportProtocolType)0xdeadbeef, &stptr);

  // Assert: unsupported protocol error
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_NULL(stptr.get);
}
