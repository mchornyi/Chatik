#include "BaseSocket.h"

#include "SocketAddress.h"

using namespace Chatik;

int
BaseSocket::Bind(const SocketAddress& inToAddress) const
{
  const int error =
    bind(mSocket, &inToAddress.mSockAddr, inToAddress.GetSize());
  if (error != 0) {
    ReportSocketError("UDPSocket::Bind");
    return GetLastSocketError();
  }

  return NO_ERROR;
}
