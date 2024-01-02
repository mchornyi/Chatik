#include "UDPSocket.h"

using namespace Chatik;

UDPSocket::~UDPSocket()
{
#if _WIN32
    closesocket(mSocket);
#else
    close(mSocket);
#endif
}

int UDPSocket::Bind(const SocketAddress &inToAddress) const
{
    const int error = bind(mSocket, &inToAddress.mSockAddr, inToAddress.GetSize());
    if (error != 0)
    {
        ReportSocketError("UDPSocket::Bind");
        return GetLastSocketError();
    }

    return NO_ERROR;
}

int UDPSocket::SendTo(const void *inToSend, int inLength, const SocketAddress &inToAddress) const
{
    const int byteSentCount = sendto(mSocket, static_cast<const char *>(inToSend), inLength, 0, &inToAddress.mSockAddr,
                                     inToAddress.GetSize());
    if (byteSentCount <= 0)
    {
        // we'll return error as negative number to indicate less than requested amount of bytes sent...
        ReportSocketError("UDPSocket::SendTo");
        return -GetLastSocketError();
    }

    return byteSentCount;
}

int UDPSocket::ReceiveFrom(void *inToReceive, int inMaxLength, SocketAddress &outFromAddress) const
{
    socklen_t fromLength = outFromAddress.GetSize();

    const int readByteCount =
        recvfrom(mSocket, static_cast<char *>(inToReceive), inMaxLength, 0, &outFromAddress.mSockAddr, &fromLength);

    if (readByteCount >= 0)
    {
        return readByteCount;
    }

    const int error = GetLastSocketError();

    if (error == WSAEWOULDBLOCK)
    {
        return 0;
    }

    if (error == WSAECONNRESET)
    {
        // this can happen if a client closed and we haven't DC'd yet.
        // this is the ICMP message being sent back saying the port on that computer is closed
        LOG("Connection reset from %s", outFromAddress.ToString().c_str());
        return -WSAECONNRESET;
    }

    ReportSocketError("UDPSocket::ReceiveFrom");
    return -error;
}

int UDPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking) const
{
#if _WIN32
    u_long arg = inShouldBeNonBlocking ? 1 : 0;
    int result = ioctlsocket(mSocket, FIONBIO, &arg);
#else
    int flags = fcntl(mSocket, F_GETFL, 0);
    flags = inShouldBeNonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    int result = fcntl(mSocket, F_SETFL, flags);
#endif

    if (result == SOCKET_ERROR)
    {
        ReportSocketError("UDPSocket::SetNonBlockingMode");
        return GetLastSocketError();
    }

    return NO_ERROR;
}
