#include "TCPSocket.h"

using namespace Chatik;

TCPSocket::~TCPSocket()
{
}

int TCPSocket::Connect(const SocketAddress &inAddress)
{
    const int err = connect(mSocket, &inAddress.mSockAddr, Chatik::SocketAddress::GetSize());
    if (err < 0)
    {
        const int errorNum = GetLastSocketError(mSocket);
        ReportSocketError("TCPSocket::Connect", errorNum);
        return errorNum;
    }

    m_serverAddress = inAddress;

    return NO_ERROR;
}

int TCPSocket::Listen(int inBackLog) const
{
    const int err = listen(mSocket, inBackLog);
    if (err < 0)
    {
        const int errorNum = GetLastSocketError(mSocket);
        ReportSocketError("TCPSocket::Listen", errorNum);
        return errorNum;
    }

    return NO_ERROR;
}

BaseSocket *TCPSocket::Accept(SocketAddress &outFromAddress) const
{
    socklen_t length = Chatik::SocketAddress::GetSize();
    const SOCKET newSocket = accept(mSocket, &outFromAddress.mSockAddr, &length);

    if (newSocket != INVALID_SOCKET)
    {
        return TCPSocketPtr(new TCPSocket(newSocket));
    }
    else
    {
        const int errorNum = GetLastSocketError(mSocket);

        if (errorNum != NO_ERROR && errorNum != WSAEWOULDBLOCK && errorNum != EAGAIN)
        {
            ReportSocketError("TCPSocket::Accept", errorNum);
        }
        return nullptr;
    }
}

int TCPSocket::Send(const void *inData, size_t inDataSize) const
{
    const int bytesSentCount = send(mSocket, static_cast<const char *>(inData), static_cast<int>(inDataSize), 0);

    if (bytesSentCount < 0)
    {
		const int errorNum = GetLastSocketError(mSocket);
        ReportSocketError("TCPSocket::Send", errorNum);
        return errorNum;
    }

    return bytesSentCount;
}

int TCPSocket::Receive(void *inBuffer, size_t inMaxLen)
{
    const int bytesReceivedCount = recv(mSocket, static_cast<char *>(inBuffer), static_cast<int>(inMaxLen), 0);
    if (bytesReceivedCount > 0)
    {
        return bytesReceivedCount;
    }

    const int errorNum = GetLastSocketError(mSocket);

    if (bytesReceivedCount == 0 && errorNum == NO_ERROR)
    {
        LOG("Connection shutdown from %s", m_serverAddress.ToString().c_str());
        mWasShutDown = true;
        return -SOCKET_CLOSED;
    }

    if (errorNum == WSAEWOULDBLOCK)
    {
        return 0;
    }

    if (errorNum == WSAECONNRESET)
    {
        // this can happen if a client closed and we haven't DC'd yet.
        // this is the ICMP message being sent back saying the port on that computer
        // is closed
        LOG("Connection reset from %s", m_serverAddress.ToString().c_str());
        mWasShutDown = true;
        return -WSAECONNRESET;
    }

    if (errorNum == WSAESHUTDOWN)
    {
        LOG("Connection shutdown from %s", m_serverAddress.ToString().c_str());
        mWasShutDown = true;
        return -WSAESHUTDOWN;
    }

    if (errorNum != NO_ERROR)
    {
        ReportSocketError("TCPSocket::Receive", errorNum);
    }

    return -errorNum;
}
