#pragma once
#include "BaseSocket.h"
#include "SocketAddress.h"

namespace Chatik
{
using TCPSocketPtr = class TCPSocket *;

class TCPSocket : public BaseSocket
{
  public:
    static BaseSocket *CreateTCPSocket(SocketAddressFamily inFamily)
    {
        const SOCKET s = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);

        if (s != INVALID_SOCKET)
        {
            return TCPSocketPtr(new TCPSocket(s));
        }
        else
        {
            ReportSocketError("SocketUtil::CreateTCPSocket", GetLastSocketError());
            return nullptr;
        }
    }

    ~TCPSocket() override;

    int Connect(const SocketAddress &inAddress) override;
    virtual int Listen(int inBackLog = 32) const override;
    virtual BaseSocket *Accept(SocketAddress &outFromAddress, int& errorNum) const override;

    virtual int Send(const void *inData, int inDataSize, const SocketAddress &inToAddress) const override
    {
        return Send(inData, inDataSize);
    }

    virtual int Receive(void *inBuffer, int inMaxLen, SocketAddress &outFromAddress) override
    {
        return Receive(inBuffer, inMaxLen);
    }

  private:
    friend class SocketUtil;
    TCPSocket(SOCKET inSocket) : BaseSocket(inSocket)
    {
    }

    int Send(const void *inData, size_t inDataSize) const;
    int Receive(void *inBuffer, size_t inMaxLen);

  private:
    SocketAddress m_serverAddress;
};

} // namespace Chatik
