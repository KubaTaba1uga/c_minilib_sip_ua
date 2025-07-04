#ifndef common_test_h
#define common_test_h

#include "c_minilib_error.h"
#include "utils/buffer_ptr.h"
#include "utils/sip_msg.h"
#include <assert.h>
#include <string.h>

static inline struct SipMessagePtr create_dummy_sipmsg(void) {
  const char *sip_request =
      "INVITE sip:bob@example.com SIP/2.0\r\n"
      "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bKbranch123\r\n"
      "Max-Forwards: 70\r\n"
      "To: <sip:bob@example.com>\r\n"
      "From: <sip:alice@example.com>;tag=1928301774\r\n"
      "Call-ID: a84b4c76e66710\r\n"
      "CSeq: 314159 INVITE\r\n"
      "Contact: <sip:alice@pc33.example.com>\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 13\r\n"
      "\r\n"
      "Hello, Bob!\r\n";

  struct SipMessagePtr sipmsg;
  struct BufferPtr buf;
  cme_error_t err;

  err = BufferPtr_create_filled(
      (csview){.buf = strdup(sip_request), .size = (long)strlen(sip_request)},
      &buf);
  assert(err == 0);

  err = SipMessagePtr_parse(buf, &sipmsg);
  assert(err == 0);

  BufferPtr_drop(&buf);

  return sipmsg;
}

#endif
