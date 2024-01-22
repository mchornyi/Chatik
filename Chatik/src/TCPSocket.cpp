#include "TCPSocket.h"

using namespace Chatik;

TCPSocket::~TCPSocket() {}

int
TCPSocket::Connect(const SocketAddress& inAddress)
{
  const int err =
    connect(mSocket, &inAddress.mSockAddr, Chatik::SocketAddress::GetSize());
  if (err < 0) {
    ReportSocketError("TCPSocket::Connect");
    return -GetLastSocketError();
  }

	m_serverAddress = inAddress;

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
TCPSocket::Receive(void* inBuffer, size_t inMaxLen)
{
  const int bytesReceivedCount =
    recv(mSocket, static_cast<char*>(inBuffer), static_cast<int>(inMaxLen), 0);

  const int error = GetLastSocketError();

  if (error == WSAEWOULDBLOCK) {
    return 0;
  }

  if (error == WSAECONNRESET) {
    // this can happen if a client closed and we haven't DC'd yet.
    // this is the ICMP message being sent back saying the port on that computer
    // is closed
    LOG("Connection reset from %s", m_serverAddress.ToString().c_str());
		mWasShutDown = true;
    return -WSAECONNRESET;
  }

  if (error == WSAESHUTDOWN) {
    LOG("Connection shutdown from %s", m_serverAddress.ToString().c_str());
		mWasShutDown = true;
    return -WSAESHUTDOWN;
  }

  if (error != NO_ERROR) {
    ReportSocketError("TCPSocket::Receive");
    return -error;
  }
  return bytesReceivedCount;
}
