#pragma once
#include "BaseSocket.h"
#include "SocketAddress.h"

namespace Chatik {
using UDPSocketPtr = class UDPSocket*;

class UDPSocket : public BaseSocket
{
public:
  static BaseSocket* CreateUDPSocket(SocketAddressFamily inFamily)
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

  virtual int Send(const void* inData,
                   int inDataSize,
                   const SocketAddress& inToAddress) const override;

  virtual int Receive(void* inBuffer,
                  int inMaxLen,
                  SocketAddress& outFromAddress) const override;

  /*
  int SendTo( const MemoryOutputStream& inMOS, const SocketAddress& inToAddress
  ); int Receive( MemoryInputStream& inMIS, SocketAddress& outFromAddress );
  */

private:
  friend class SocketUtil;
  UDPSocket(SOCKET inSocket)
    : BaseSocket(inSocket)
  {
  }
};
} // namespace Chatik
