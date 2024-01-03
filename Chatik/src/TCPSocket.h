#pragma once
#include "BaseSocket.h"
#include "SocketAddress.h"

namespace Chatik {
using TCPSocketPtr = class TCPSocket*;

class TCPSocket : public BaseSocket
{
public:
  static TCPSocketPtr CreateTCPSocket(SocketAddressFamily inFamily)
  {
    const SOCKET s = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);

    if (s != INVALID_SOCKET) {
      return TCPSocketPtr(new TCPSocket(s));
    } else {
      ReportSocketError("SocketUtil::CreateTCPSocket");
      return nullptr;
    }
  }

  ~TCPSocket();

  int Connect(const SocketAddress& inAddress) const;
  int Bind(const SocketAddress& inBindAddress) const;
  int Listen(int inBackLog = 32) const;

  TCPSocketPtr Accept(SocketAddress& inFromAddress) const;
  int32_t Send(const void* inData, size_t inLen) const;
  int32_t Receive(void* inData, size_t inLen) const;

private:
  friend class SocketUtil;
  TCPSocket(SOCKET inSocket)
    : BaseSocket(inSocket)
  {
  }
};

} // namespace Chatik
