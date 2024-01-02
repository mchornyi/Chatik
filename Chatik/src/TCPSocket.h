#pragma once
#include "SocketAddress.h"

namespace Chatik
{
using TCPSocketPtr = class TCPSocket *;

class TCPSocket
{
  public:
    ~TCPSocket();

    int Connect(const SocketAddress &inAddress) const;
    int Bind(const SocketAddress &inBindAddress) const;
    int Listen(int inBackLog = 32) const;

    TCPSocketPtr Accept(SocketAddress &inFromAddress) const;
    int32_t Send(const void *inData, size_t inLen) const;
    int32_t Receive(void *inData, size_t inLen) const;

  private:
    friend class SocketUtil;
    TCPSocket(SOCKET inSocket) : mSocket(inSocket)
    {
    }

  private:
    SOCKET mSocket;
};

} // namespace Chatik
