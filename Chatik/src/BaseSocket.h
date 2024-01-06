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

  bool IsValid() const
  {
    int error;
    socklen_t len = sizeof(error);

    const int ret =
      getsockopt(mSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

    if (ret == 0 && error == 0) {
      // Socket is connected
      return true;
    }

    // Socket is not connected
    return false;
  }

protected:
  SOCKET mSocket = INVALID_SOCKET;
};
}
