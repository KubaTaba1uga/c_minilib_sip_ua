#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "unity_wrapper.h"

#include "socket/socket.h"

static cmsu_sock_list_t sockets = NULL;
static cmsu_sock_t sock = NULL;
static int dummy_fd = -1;
static int recv_called = 0;
static int send_called = 0;

static cme_error_t dummy_recv(uint32_t buf_len, char *buf, void *data) {
  recv_called++;
  (void)buf_len;
  (void)buf;
  (void)data;
  return NULL;
}

static cme_error_t dummy_send(uint32_t buf_len, char *buf, void *data) {
  send_called++;
  (void)buf_len;
  (void)buf;
  (void)data;
  return NULL;
}

static void dummy_destroy(void *data) {}

int cmsu_SocketUdp_recvfrom(int sockfd, void *buf, size_t len, int flags,
                            struct sockaddr *src_addr, socklen_t *addrlen) {
  return 100;
}

void setUp(void) {
  cme_init();
  recv_called = 0;
  send_called = 0;
  sockets = NULL;
  sock = NULL;
}

void tearDown(void) {
  if (sockets) {
    cmsu_sock_list_destroy(&sockets);
  }
}

void test_create_udp_socket_and_insert_to_list(void) {
  cme_error_t err = cmsu_sock_list_create(&sockets);
  MYTEST_ASSERT_ERR_NULL(err);
  TEST_ASSERT_NOT_NULL(sockets);

  err = cmsu_sock_list_insert_udp("127.0.0.1", 5070,
                                  (cmsu_sock_arg_t){.ctx = NULL,
                                                    .recvh = dummy_recv,
                                                    .sendh = dummy_send,
                                                    .destroyh = dummy_destroy},
                                  &sock, sockets);
  MYTEST_ASSERT_ERR_NULL(err);
  TEST_ASSERT_NOT_NULL(sock);

  int fd = cmsu_sock_get_fd(sock);
  TEST_ASSERT_GREATER_THAN(-1, fd);
  dummy_fd = fd;

  cmsu_sock_t found = cmsu_sock_list_find(fd, sockets);
  TEST_ASSERT_EQUAL_PTR(sock, found);
}

void test_recv_and_send_callbacks_are_called(void) {
  cme_error_t err = cmsu_sock_list_create(&sockets);
  MYTEST_ASSERT_ERR_NULL(err);
  err = cmsu_sock_list_insert_udp("127.0.0.1", 5070,
                                  (cmsu_sock_arg_t){.ctx = NULL,
                                                    .recvh = dummy_recv,
                                                    .sendh = dummy_send,
                                                    .destroyh = dummy_destroy},
                                  &sock, sockets);
  MYTEST_ASSERT_ERR_NULL(err);

  err = cmsu_sock_recv(sock);
  MYTEST_ASSERT_ERR_NULL(err);
  err = cmsu_sock_send(sock);
  MYTEST_ASSERT_ERR_NULL(err);

  TEST_ASSERT_EQUAL(1, recv_called);
  TEST_ASSERT_EQUAL(1, send_called);
}

void test_invalid_fd_lookup_returns_null(void) {
  cme_error_t err = cmsu_sock_list_create(&sockets);
  MYTEST_ASSERT_ERR_NULL(err);

  cmsu_sock_t found = cmsu_sock_list_find(-123, sockets);
  TEST_ASSERT_NULL(found);
}

void test_valid_fd_lookup_returns_sock(void) {
  cme_error_t err = cmsu_sock_list_create(&sockets);
  MYTEST_ASSERT_ERR_NULL(err);

  err = cmsu_sock_list_insert_udp("127.0.0.1", 5071,
                                  (cmsu_sock_arg_t){.ctx = NULL,
                                                    .recvh = dummy_recv,
                                                    .sendh = dummy_send,
                                                    .destroyh = dummy_destroy},
                                  &sock, sockets);
  MYTEST_ASSERT_ERR_NULL(err);
  TEST_ASSERT_NOT_NULL(sock);

  int fd = cmsu_sock_get_fd(sock);
  TEST_ASSERT_GREATER_THAN(-1, fd);

  cmsu_sock_t found = cmsu_sock_list_find(fd, sockets);
  TEST_ASSERT_NOT_NULL(found);
  TEST_ASSERT_EQUAL_PTR(sock, found);
}
