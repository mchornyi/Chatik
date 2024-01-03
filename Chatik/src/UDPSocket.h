#pragma once
#include "BaseSocket.h"
#include "SocketAddress.h"

namespace Chatik {
using UDPSocketPtr = class UDPSocket*;

class UDPSocket : public BaseSocket
{
public:
  static UDPSocketPtr CreateUDPSocket(SocketAddressFamily inFamily)
  {
    const SOCKET s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);

    if (s != INVALID_SOCKET) {
      return UDPSocketPtr(new UDPSocket(s));
    } else {
      ReportSocketError("SocketUtil::CreateUDPSocket");
      return nullptr;
    }
  }

  ~UDPSocket();

  int Bind(const SocketAddress& inToAddress) const;

  int SendTo(const void* inToSend,
             int inLength,
             const SocketAddress& inToAddress) const;

  int ReceiveFrom(void* inToReceive,
                  int inMaxLength,
                  SocketAddress& outFromAddress) const;

  /*
  int SendTo( const MemoryOutputStream& inMOS, const SocketAddress& inToAddress
  ); int ReceiveFrom( MemoryInputStream& inMIS, SocketAddress& outFromAddress );
  */

  int SetNonBlockingMode(bool inShouldBeNonBlocking) const;

private:
  friend class SocketUtil;
  UDPSocket(SOCKET inSocket)
    : BaseSocket(inSocket)
  {
  }
};
} // namespace Chatik
