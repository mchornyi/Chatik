#include "UDPSocket.h"

using namespace Chatik;

UDPSocket::~UDPSocket() {}

int
UDPSocket::Send(const void* inData,
                int inDataSize,
                const SocketAddress& inToAddress) const
{
  const int byteSentCount = sendto(mSocket,
                                   static_cast<const char*>(inData),
                                   inDataSize,
                                   0,
                                   &inToAddress.mSockAddr,
                                   Chatik::SocketAddress::GetSize());
  if (byteSentCount <= 0) {
    // we'll return error as negative number to indicate less than requested
    // amount of bytes sent...
    ReportSocketError("UDPSocket::SendTo");
    return -GetLastSocketError();
  }

  return byteSentCount;
}

int
UDPSocket::Receive(void* inBuffer,
                   int inMaxLen,
                   SocketAddress& outFromAddress) const
{
  socklen_t fromLength = Chatik::SocketAddress::GetSize();

  const int readByteCount = recvfrom(mSocket,
                                     static_cast<char*>(inBuffer),
                                     inMaxLen,
                                     0,
                                     &outFromAddress.mSockAddr,
                                     &fromLength);

  if (readByteCount >= 0) {
    return readByteCount;
  }

  const int error = GetLastSocketError();

  if (error == WSAEWOULDBLOCK) {
    return 0;
  }

  if (error == WSAECONNRESET) {
    // this can happen if a client closed and we haven't DC'd yet.
    // this is the ICMP message being sent back saying the port on that computer
    // is closed
    LOG("Connection reset from %s", outFromAddress.ToString().c_str());
    return -WSAECONNRESET;
  }

  if (error == WSAESHUTDOWN) {
    LOG("Connection shutdown from %s", outFromAddress.ToString().c_str());
    return -WSAESHUTDOWN;
  }

  ReportSocketError("UDPSocket::Receive");
  return -error;
}
