#include "BaseSocket.h"

#include "SocketAddress.h"

using namespace Chatik;

BaseSocket::~BaseSocket()
{
#if _WIN32
  const int res = closesocket(mSocket);
  assert(res == NO_ERROR);
#else
  const int res = close(mSocket);
  assert(res == NO_ERROR);
#endif
}

int
BaseSocket::Bind(const SocketAddress& inToAddress) const
{
  const int error =
    bind(mSocket, &inToAddress.mSockAddr, Chatik::SocketAddress::GetSize());
  if (error != 0) {
    ReportSocketError("UDPSocket::Bind");
    return GetLastSocketError();
  }

  return NO_ERROR;
}

bool
BaseSocket::ShutDown()
{
  assert(!mIsShutDown);

  if (mIsShutDown) {
    return true;
  }

  shutdown(GetSocket(), SD_SEND);

  const int errorNum = GetLastSocketError();
  assert(errorNum == NO_ERROR);
  const bool res = (errorNum == NO_ERROR);
  if (errorNum != NO_ERROR) {
    ReportSocketError("BaseSocket::ShutDown");
  }

  mIsShutDown = res;

  return mIsShutDown;
}
