#include "../src/SocketUtil.h"
#include "UnitTest++/UnitTest++.h"

using namespace Chatik;

SUITE(SocketUtilTest)
{
  TEST(CreateUDPSocketTest)
  {
    UDPSocketPtr s = UDPSocket::CreateUDPSocket(INET);
    REQUIRE CHECK(s != nullptr);

    int res = -1;
    int iSocketType = 0;
    int iProtocol = 0;
#if _WIN32
    WSAPROTOCOL_INFO socket_info{};
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

    REQUIRE CHECK_EQUAL(0, res);
    CHECK_EQUAL(SOCK_DGRAM, iSocketType);
    CHECK_EQUAL(IPPROTO_UDP, iProtocol);

    delete s;
  }

  TEST(CreateTCPSocketTest)
  {
    TCPSocketPtr s = TCPSocket::CreateTCPSocket(INET);
    REQUIRE CHECK(s != nullptr);

    int res = -1;
    int iSocketType = 0;
    int iProtocol = 0;
#if _WIN32
    WSAPROTOCOL_INFO socket_info{};
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

    REQUIRE CHECK_EQUAL(0, res);
    CHECK_EQUAL(SOCK_STREAM, iSocketType);
    CHECK_EQUAL(IPPROTO_TCP, iProtocol);

    delete s;
  }
}
