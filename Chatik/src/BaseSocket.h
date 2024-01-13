#pragma once
#include "ChatikCommon.h"

namespace Chatik {
class SocketAddress;

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

  int SetNonBlockingMode(bool inShouldBeNonBlocking) const
  {
#if _WIN32
    u_long arg = inShouldBeNonBlocking ? 1 : 0;
    int result = ioctlsocket(mSocket, FIONBIO, &arg);
#else
    int flags = fcntl(mSocket, F_GETFL, 0);
    flags =
      inShouldBeNonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    int result = fcntl(mSocket, F_SETFL, flags);
#endif

    if (result == SOCKET_ERROR) {
      ReportSocketError("UDPSocket::SetNonBlockingMode");
      return GetLastSocketError();
    }

    return NO_ERROR;
  }

  int Bind(const SocketAddress& inToAddress) const;

  virtual int Send(const void* inData,
                   int inDataSize,
                   const SocketAddress& inToAddress) const
  {
    return -1;
  }

  virtual int Receive(void* inBuffer,
                      int inMaxLen,
                      SocketAddress& outFromAddress) const
  {
    return -1;
  }

  virtual int Listen(int inBackLog = 32) const { return -1; }

  virtual BaseSocket* Accept(SocketAddress& inFromAddress) const
  {
    return nullptr;
  }

protected:
  SOCKET mSocket = INVALID_SOCKET;
};
}
