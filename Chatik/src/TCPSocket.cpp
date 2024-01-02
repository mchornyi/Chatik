#include "TCPSocket.h"

using namespace Chatik;

TCPSocket::~TCPSocket()
{
#if _WIN32
    closesocket(mSocket);
#else
    close(mSocket);
#endif
}

int TCPSocket::Connect(const SocketAddress& inAddress) const
{
    const int err = connect(mSocket, &inAddress.mSockAddr, inAddress.GetSize());
    if (err < 0)
    {
        ReportSocketError("TCPSocket::Connect");
        return -GetLastSocketError();
    }
    return NO_ERROR;
}

int TCPSocket::Listen(int inBackLog) const
{
    const int err = listen(mSocket, inBackLog);
    if (err < 0)
    {
        ReportSocketError("TCPSocket::Listen");
        return -GetLastSocketError();
    }

    return NO_ERROR;
}

TCPSocketPtr TCPSocket::Accept(SocketAddress& inFromAddress) const
{
    socklen_t length = inFromAddress.GetSize();
    const SOCKET newSocket = accept(mSocket, &inFromAddress.mSockAddr, &length);

    if (newSocket != INVALID_SOCKET)
    {
        return TCPSocketPtr(new TCPSocket(newSocket));
    }
    else
    {
        ReportSocketError("TCPSocket::Accept");
        return nullptr;
    }
}

int32_t TCPSocket::Send(const void* inData, size_t inLen) const
{
    const int bytesSentCount = send(mSocket, static_cast<const char*>(inData), inLen, 0);

    if (bytesSentCount < 0)
    {
        ReportSocketError("TCPSocket::Send");
        return -GetLastSocketError();
    }

    return bytesSentCount;
}

int32_t TCPSocket::Receive(void* inData, size_t inLen) const
{
    const int bytesReceivedCount = recv(mSocket, static_cast<char*>(inData), inLen, 0);

    if (bytesReceivedCount < 0)
    {
        ReportSocketError("TCPSocket::Receive");
        return -GetLastSocketError();
    }

    return bytesReceivedCount;
}

int TCPSocket::Bind(const SocketAddress& inBindAddress) const
{
    const int error = bind(mSocket, &inBindAddress.mSockAddr, inBindAddress.GetSize());

    if (error != 0)
    {
        ReportSocketError("TCPSocket::Bind");
        return GetLastSocketError();
    }

    return NO_ERROR;
}
