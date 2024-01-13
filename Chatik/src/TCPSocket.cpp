#include "TCPSocket.h"

using namespace Chatik;

TCPSocket::~TCPSocket() {}

int
TCPSocket::Connect(const SocketAddress& inAddress) const
{
  const int err =
    connect(mSocket, &inAddress.mSockAddr, Chatik::SocketAddress::GetSize());
  if (err < 0) {
    ReportSocketError("TCPSocket::Connect");
    return -GetLastSocketError();
  }
  return NO_ERROR;
}

int
TCPSocket::Listen(int inBackLog) const
{
  const int err = listen(mSocket, inBackLog);
  if (err < 0) {
    ReportSocketError("TCPSocket::Listen");
    return -GetLastSocketError();
  }

  return NO_ERROR;
}

BaseSocket*
TCPSocket::Accept(SocketAddress& outFromAddress) const
{
  socklen_t length = Chatik::SocketAddress::GetSize();
  const SOCKET newSocket = accept(mSocket, &outFromAddress.mSockAddr, &length);

  if (newSocket != INVALID_SOCKET) {
    return TCPSocketPtr(new TCPSocket(newSocket));
  } else {
    const int errorNum = GetLastSocketError();

    if (errorNum != WSAEWOULDBLOCK && errorNum != EAGAIN) {
      ReportSocketError("TCPSocket::Accept");
    }
    return nullptr;
  }
}

int
TCPSocket::Send(const void* inData, size_t inDataSize) const
{
  const int bytesSentCount = send(
    mSocket, static_cast<const char*>(inData), static_cast<int>(inDataSize), 0);

  if (bytesSentCount < 0) {
    ReportSocketError("TCPSocket::Send");
    return -GetLastSocketError();
  }

  return bytesSentCount;
}

int
TCPSocket::Receive(void* inBuffer, size_t inMaxLen) const
{
  const int bytesReceivedCount =
    recv(mSocket, static_cast<char*>(inBuffer), static_cast<int>(inMaxLen), 0);

  if (bytesReceivedCount < 0) {
    ReportSocketError("TCPSocket::Receive");
    return -GetLastSocketError();
  }

  return bytesReceivedCount;
}
