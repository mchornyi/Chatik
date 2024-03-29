#include "UDPSocket.h"

using namespace Chatik;

UDPSocket::~UDPSocket()
{
}

int UDPSocket::Send(const void *inData, int inDataSize, const SocketAddress &inToAddress) const
{
    const int byteSentCount = sendto(mSocket, static_cast<const char *>(inData), inDataSize, 0, &inToAddress.mSockAddr,
                                     Chatik::SocketAddress::GetSize());
    if (byteSentCount <= 0)
    {
        // we'll return error as negative number to indicate less than requested
        // amount of bytes sent...
		const int errorNum = GetLastSocketError(mSocket);
        ReportSocketError("UDPSocket::SendTo", errorNum);
        return -errorNum;
    }

    return byteSentCount;
}

int UDPSocket::Receive(void *inBuffer, int inMaxLen, SocketAddress &outFromAddress)
{
    socklen_t fromLength = SocketAddress::GetSize();

    const int readByteCount =
        recvfrom(mSocket, static_cast<char *>(inBuffer), inMaxLen, 0, &outFromAddress.mSockAddr, &fromLength);

    if (readByteCount >= 0)
    {
        return readByteCount;
    }

    const int errorNum = GetLastSocketError(mSocket);

    if (errorNum == WSAEWOULDBLOCK)
    {
        return 0;
    }

    if (errorNum == WSAECONNRESET)
    {
        // this can happen if a client closed and we haven't DC'd yet.
        // this is the ICMP message being sent back saying the port on that computer
        // is closed
        LOG("Connection reset from %s", outFromAddress.ToString().c_str());
        mWasShutDown = true;
        return -WSAECONNRESET;
    }

    if (errorNum == WSAESHUTDOWN)
    {
        LOG("Connection shutdown from %s", outFromAddress.ToString().c_str());
        mWasShutDown = true;
        return -WSAESHUTDOWN;
    }

    ReportSocketError("UDPSocket::Receive", errorNum);
    return -errorNum;
}
