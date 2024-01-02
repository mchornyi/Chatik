#include "SocketAddress.h"

#if !_WIN32
#include <arpa/inet.h>
#endif

using namespace Chatik;

string SocketAddress::ToString() const
{
    const sockaddr_in* s = GetAsSockAddrIn();
    char destinationBuffer[128];

#if _WIN32
    InetNtop(s->sin_family, const_cast<in_addr *>(&s->sin_addr), destinationBuffer, sizeof(destinationBuffer));
#else
    inet_ntop(s->sin_family, const_cast<in_addr *>(&s->sin_addr), destinationBuffer, sizeof(destinationBuffer));
#endif

    return Sprintf("%s:%d", destinationBuffer, ntohs(s->sin_port));
}
