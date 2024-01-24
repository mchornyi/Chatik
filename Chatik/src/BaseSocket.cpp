#include "BaseSocket.h"

#include <cassert>
#include <sstream>

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

int BaseSocket::Bind(const SocketAddress &inToAddress) const
{
    const int error = bind(mSocket, &inToAddress.mSockAddr, Chatik::SocketAddress::GetSize());
    if (error != 0)
    {
        const int errorNum = GetLastSocketError();
        std::stringstream ss;
        ss << "BaseSocket::Bind: " << inToAddress.ToString();
        ReportSocketError(ss.str().c_str(), errorNum);
        return -errorNum;
    }

    return NO_ERROR;
}

bool BaseSocket::ShutDown()
{
    assert(!mIsShutDown);

    if (mIsShutDown)
    {
        return true;
    }

    shutdown(GetSocketHandle(), SD_SEND);

    const int errorNum = GetLastSocketError(mSocket);
    const bool res = (errorNum == NO_ERROR);
    if (!res)
    {
        ReportSocketError("BaseSocket::ShutDown", errorNum);
    }

    mIsShutDown = res;

    return mIsShutDown;
}
