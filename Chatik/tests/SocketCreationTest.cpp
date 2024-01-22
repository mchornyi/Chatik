#include "../src/SocketUtil.h"
#include "../src/UDPSocket.h"
#include "catch2/catch_test_macros.hpp"

using namespace Chatik;

  TEST_CASE("CreateUDPSocketTest", "[udp]")
  {
    BaseSocket* s = UDPSocket::CreateUDPSocket(INET);
    REQUIRE(s != nullptr);

    CHECK(true == s->IsValid());

    int res = -1;
    int iSocketType = 0;
    int iProtocol = 0;
#if _WIN32
    WSAPROTOCOL_INFOW socket_info{};
    int size = sizeof(socket_info);

    res = getsockopt(
      s->GetSocket(), SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&socket_info, &size);
    if (res != 0) {
      ReportSocketError("CreateUDPSocketTest");
    }

    iSocketType = socket_info.iSocketType;
    iProtocol = socket_info.iProtocol;
#else
    {
      socklen_t size = sizeof(iSocketType);
      res = getsockopt(
        s->GetSocket(), SOL_SOCKET, SO_TYPE, &iSocketType, &size);
      if (res != 0) {
        ReportSocketError("CreateUDPSocketTest");
      }
    }

    {
      socklen_t size = sizeof(iSocketType);
      res = getsockopt(
        s->GetSocket(), SOL_SOCKET, SO_PROTOCOL, &iProtocol, &size);
      if (res != 0) {
        ReportSocketError("CreateUDPSocketTest");
      }
    }
#endif

    REQUIRE(0 == res);
    CHECK(SOCK_DGRAM == iSocketType);
    CHECK(IPPROTO_UDP == iProtocol);

    delete s;
  }

	TEST_CASE("CreateTCPSocketTest", "[tcp]")
  {
    BaseSocket* s = TCPSocket::CreateTCPSocket(INET);
    REQUIRE(s != nullptr);

    CHECK(true == s->IsValid());

    int res = -1;
    int iSocketType = 0;
    int iProtocol = 0;
#if _WIN32
    WSAPROTOCOL_INFOW socket_info{};
    int size = sizeof(socket_info);

    res = getsockopt(
      s->GetSocket(), SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&socket_info, &size);
    if (res != 0) {
      ReportSocketError("CreateTCPSocketTest");
    }

    iSocketType = socket_info.iSocketType;
    iProtocol = socket_info.iProtocol;
#else
    {
      socklen_t size = sizeof(iSocketType);
      res = getsockopt(
        s->GetSocket(), SOL_SOCKET, SO_TYPE, &iSocketType, &size);
      if (res != 0) {
        ReportSocketError("CreateTCPSocketTest");
      }
    }

    {
      socklen_t size = sizeof(iSocketType);
      res = getsockopt(
        s->GetSocket(), SOL_SOCKET, SO_PROTOCOL, &iProtocol, &size);
      if (res != 0) {
        ReportSocketError("CreateTCPSocketTest");
      }
    }
#endif

    REQUIRE(0 == res);
    CHECK(SOCK_STREAM == iSocketType);
    CHECK(IPPROTO_TCP == iProtocol);

    delete s;
  }

	TEST_CASE("SetNonBlockingUDPSocketTest")
  {
    BaseSocket* s = UDPSocket::CreateUDPSocket(INET);
    REQUIRE(s != nullptr);

    int res = s->SetNonBlockingMode(true);
    CHECK(NO_ERROR == res);

    res = s->SetNonBlockingMode(false);
    CHECK(NO_ERROR == res);

    delete s;
  }
