#pragma once
#include "SocketAddress.h"

namespace Chatik
{
using UDPSocketPtr = class UDPSocket *;

class UDPSocket
{
  public:
    ~UDPSocket();

    int Bind(const SocketAddress &inToAddress) const;

    int SendTo(const void *inToSend, int inLength, const SocketAddress &inToAddress) const;

    int ReceiveFrom(void *inToReceive, int inMaxLength, SocketAddress &outFromAddress) const;

    /*
    int SendTo( const MemoryOutputStream& inMOS, const SocketAddress& inToAddress );
    int ReceiveFrom( MemoryInputStream& inMIS, SocketAddress& outFromAddress );
    */

    int SetNonBlockingMode(bool inShouldBeNonBlocking) const;

  private:
    friend class SocketUtil;
    UDPSocket(SOCKET inSocket) : mSocket(inSocket)
    {
    }

  private:
    SOCKET mSocket;
};
} // namespace Chatik
