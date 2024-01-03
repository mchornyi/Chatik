#pragma once
#include "ChatikCommon.h"

namespace Chatik {
class BaseSocket
{
public:
  BaseSocket(SOCKET inSocket)
    : mSocket(inSocket)
  {
  }

  virtual ~BaseSocket()
  {
#if _WIN32
    closesocket(mSocket);
#else
    close(mSocket);
#endif
  }

  SOCKET GetSocket() const { return mSocket; }

protected:
  SOCKET mSocket = INVALID_SOCKET;
};
}
