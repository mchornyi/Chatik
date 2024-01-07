#include "SocketAddress.h"

#if !_WIN32
#include <arpa/inet.h>
#endif

using namespace Chatik;

string
SocketAddress::ToString() const
{
  char destinationBuffer[128];

#if _WIN32
  InetNtop(mSockAddrRef.sin_family,
           &mSockAddrRef.sin_addr,
           destinationBuffer,
           sizeof(destinationBuffer));
#else
  inet_ntop(mSockAddrRef.sin_family,
            const_cast<in_addr*>(&mSockAddrRef.sin_addr),
            destinationBuffer,
            sizeof(destinationBuffer));
#endif

  return Sprintf("%s:%d", destinationBuffer, ntohs(mSockAddrRef.sin_port));
}
