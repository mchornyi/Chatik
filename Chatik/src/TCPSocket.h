#pragma once
#include "BaseSocket.h"
#include "SocketAddress.h"

namespace Chatik {
using TCPSocketPtr = class TCPSocket*;

class TCPSocket : public BaseSocket
{
public:
  static BaseSocket* CreateTCPSocket(SocketAddressFamily inFamily)
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

  int Connect(const SocketAddress& inAddress) const override;
  virtual int Listen(int inBackLog = 32) const override;
  virtual BaseSocket* Accept(SocketAddress& outFromAddress) const override;

  virtual int Send(const void* inData,
                   int inDataSize,
                   const SocketAddress& inToAddress) const override
  {
    return Send(inData, inDataSize);
  }

  virtual int Receive(void* inBuffer,
                      int inMaxLen,
                      SocketAddress& outFromAddress) const override
  {
    return Receive(inBuffer, inMaxLen);
  }

private:
  friend class SocketUtil;
  TCPSocket(SOCKET inSocket)
    : BaseSocket(inSocket)
  {
  }

  int Send(const void* inData, size_t inDataSize) const;
  int Receive(void* inBuffer, size_t inMaxLen) const;
};

} // namespace Chatik
