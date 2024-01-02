#include "SocketAddressFactory.h"

using namespace Chatik;

SocketAddressPtr SocketAddressFactory::CreateIPv4FromString(const string &inString)
{
    const auto pos = inString.find_last_of(':');
    string host, service;
    if (pos != string::npos)
    {
        host = inString.substr(0, pos);
        service = inString.substr(pos + 1);
    }
    else
    {
        host = inString;
        // use default port...
        service = "0";
    }

    addrinfo hint = {};
    hint.ai_family = AF_INET;

    addrinfo *result = nullptr;
    const int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
    if (error != 0)
    {
        ReportSocketError("SocketAddressFactory::CreateIPv4FromString");
        return nullptr;
    }

    if (!result->ai_addr)
    {
        return nullptr;
    }

    freeaddrinfo(result);

    return new SocketAddress(*result->ai_addr);
}
