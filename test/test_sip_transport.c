// test/test_sip_transport.c

#include <arpa/inet.h>
#include <assert.h>
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

static struct EventLoopPtr evlp;
static struct IpAddrPtr ip;
static struct SipTransportPtr stptr;
static bool __UdpSocket_create_fd_mock_err;

cme_error_t __UdpSocket_create_fd(struct IpAddrPtr ip_addr, int32_t *fd) {
  (void)ip_addr;
  if (__UdpSocket_create_fd_mock_err) {
    return cme_error(EINVAL, "mocked failure in __UdpSocket_create_fd");
  }
  *fd = 13;
  return 0;
}

void setUp(void) {
  cme_init();

  cme_error_t err = EventLoopPtr_create(&evlp);
  MYTEST_ASSERT_ERR_NULL(err);

  ip = (struct IpAddrPtr){0};
  stptr = (struct SipTransportPtr){0};
  __UdpSocket_create_fd_mock_err = true;
}

void tearDown(void) {
  if (stptr.get) {
    SipTransportPtr_drop(&stptr);
  }
  if (ip.get) {
    IpAddrPtr_drop(&ip);
  }
  if (evlp.get) {
    EventLoopPtr_drop(&evlp);
  }
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
  MYTEST_ASSERT_ERR_NOT_NULL(err);
  TEST_ASSERT_NULL(stptr.get);
}

#include "sip_transport/_internal/sip_transport_listen.h"
#include "utils/buffer_ptr.h"
#include "utils/sip_msg.h"

// A minimal valid SIP INVITE for testing
static const char *kValidInvite = "INVITE sip:bob@example.com SIP/2.0\r\n"
                                  "Via: SIP/2.0/UDP host;branch=z9hG4bK1\r\n"
                                  "To: <sip:bob@example.com>\r\n"
                                  "From: <sip:alice@example.com>;tag=1\r\n"
                                  "Call-ID: 1\r\n"
                                  "CSeq: 1 INVITE\r\n"
                                  "Content-Length: 0\r\n"
                                  "\r\n";

// Globals for handler observation
static bool g_handler_called;
static struct GenericPtr g_handler_arg;

// A test handler that simply records it was called
static cme_error_t test_sip_reqh_ok(struct SipMessagePtr msg,
                                    struct IpAddrPtr peer,
                                    struct SipTransportPtr st,
                                    struct GenericPtr arg) {
  (void)msg;
  (void)peer;
  (void)st;
  g_handler_called = true;
  g_handler_arg = arg;
  return 0;
}

// A test handler that always fails
static cme_error_t test_sip_reqh_err(struct SipMessagePtr msg,
                                     struct IpAddrPtr peer,
                                     struct SipTransportPtr st,
                                     struct GenericPtr arg) {
  (void)msg;
  (void)peer;
  (void)st;
  (void)arg;
  return cme_error(EIO, "handler error");
}

void test___SipTransport_udp_recvh_valid_message(void) {
  cme_error_t err;

  __UdpSocket_create_fd_mock_err = false;
  ip = IpAddrPtr_create(cstr_lit("127.0.0.1"), cstr_lit("5060"));
  err = SipTransportPtr_create(evlp, ip, SipTransportProtocolType_UDP, &stptr);
  MYTEST_ASSERT_ERR_NULL(err);

  stptr.get->recvh = test_sip_reqh_ok;
  stptr.get->recvh_arg = GenericPtr_from_arc(SipTransportPtr, &stptr);
  g_handler_called = false;

  struct BufferPtr bufp;
  size_t len = strlen(kValidInvite);

  err = BufferPtr_create_filled(
      (struct csview){.size = (long)len, .buf = strdup(kValidInvite)}, &bufp);
  MYTEST_ASSERT_ERR_NULL(err);

  err = __SipTransport_udp_recvh(bufp, ip,
                                 GenericPtr_from_arc(SipTransportPtr, &stptr));
  MYTEST_ASSERT_ERR_NULL(err);

  TEST_ASSERT_TRUE(g_handler_called);
  struct SipTransportPtr unpack =
      GenericPtr_dump(SipTransportPtr, g_handler_arg);
  TEST_ASSERT_EQUAL_PTR(stptr.get, unpack.get);

  BufferPtr_drop(&bufp);
}

void test___SipTransport_udp_recvh_malformed_message_ignored(void) {

  cme_error_t err;

  __UdpSocket_create_fd_mock_err = false;
  ip = IpAddrPtr_create(cstr_lit("127.0.0.1"), cstr_lit("5060"));
  err = SipTransportPtr_create(evlp, ip, SipTransportProtocolType_UDP, &stptr);
  MYTEST_ASSERT_ERR_NULL(err);

  stptr.get->recvh = test_sip_reqh_ok;
  stptr.get->recvh_arg = GenericPtr_from_arc(SipTransportPtr, &stptr);
  g_handler_called = false;

  const char *bad = "NOT SIP";
  struct BufferPtr bufp;
  err = BufferPtr_create_filled(
      (struct csview){.size = (long)strlen(bad), .buf = strdup(bad)}, &bufp);
  MYTEST_ASSERT_ERR_NULL(err);

  err = __SipTransport_udp_recvh(bufp, ip,
                                 GenericPtr_from_arc(SipTransportPtr, &stptr));
  MYTEST_ASSERT_ERR_NULL(err);

  TEST_ASSERT_FALSE(g_handler_called);

  BufferPtr_drop(&bufp);
}

void test___SipTransport_udp_recvh_handler_error_propagates(void) {
  // Arrange: same transport
  __UdpSocket_create_fd_mock_err = false;
  ip = IpAddrPtr_create(cstr_lit("127.0.0.1"), cstr_lit("5060"));
  cme_error_t err =
      SipTransportPtr_create(evlp, ip, SipTransportProtocolType_UDP, &stptr);
  MYTEST_ASSERT_ERR_NULL(err);

  stptr.get->recvh = test_sip_reqh_err;
  stptr.get->recvh_arg = GenericPtr_from_arc(SipTransportPtr, &stptr);

  // Build valid SIP buffer
  struct BufferPtr bufp;
  err = BufferPtr_create_filled(
      (struct csview){.size = (long)strlen(kValidInvite),
                      .buf = strdup(kValidInvite)},
      &bufp);
  MYTEST_ASSERT_ERR_NULL(err);

  // Act
  err = __SipTransport_udp_recvh(bufp, ip,
                                 GenericPtr_from_arc(SipTransportPtr, &stptr));

  // Assert: our handler’s EIO gets propagated
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL_INT(EIO, err->code);

  BufferPtr_drop(&bufp);
}
