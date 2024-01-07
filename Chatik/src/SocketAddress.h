#pragma once
#include "ChatikCommon.h"

namespace Chatik {
class SocketAddress
{
public:
  SocketAddress(uint32_t inAddress, uint16_t inPort)
    : mSockAddrRef(reinterpret_cast<sockaddr_in&>(mSockAddr))
  {
    mSockAddrRef.sin_family = AF_INET;
    SetIP4(htonl(inAddress));
    mSockAddrRef.sin_port = htons(inPort);
  }

  SocketAddress(const sockaddr& inSockAddr)
    : mSockAddrRef(reinterpret_cast<sockaddr_in&>(mSockAddr))
  {
    memcpy(&mSockAddr, &inSockAddr, sizeof(sockaddr));
  }

  SocketAddress()
    : mSockAddrRef(reinterpret_cast<sockaddr_in&>(mSockAddr))
  {
    mSockAddrRef.sin_family = AF_INET;
    SetIP4(INADDR_ANY);
    mSockAddrRef.sin_port = 0;
  }

  bool operator==(const SocketAddress& inOther) const
  {
    return (mSockAddr.sa_family == AF_INET &&
            mSockAddrRef.sin_port == inOther.mSockAddrRef.sin_port) &&
           (GetIP4() == inOther.GetIP4());
  }

  bool IsValid() const
  {
    bool res = (mSockAddr.sa_family == AF_INET);
    res &= (GetIP4() != INADDR_ANY);
    return res;
  }

  size_t GetHash() const
  {
    return (GetIP4()) | ((static_cast<uint32_t>(mSockAddrRef.sin_port)) << 13) |
           mSockAddr.sa_family;
  }

  static uint32_t GetSize() { return sizeof(sockaddr); }

  string ToString() const;

private:
  friend class UDPSocket;
  friend class TCPSocket;

#if _WIN32
  uint32_t GetIP4() const { return mSockAddrRef.sin_addr.s_addr; }
  void SetIP4(ULONG value) { mSockAddrRef.sin_addr.s_addr = value; }
#else
  uint32_t GetIP4() const { return mSockAddrRef.sin_addr.s_addr; }
  void SetIP4(uint32_t value) { mSockAddrRef.sin_addr.s_addr = value; }
#endif

private:
  sockaddr mSockAddr;
  sockaddr_in& mSockAddrRef;
};

typedef SocketAddress* SocketAddressPtr;

} // namespace Chatik

namespace std {
template<>
struct hash<Chatik::SocketAddress>
{
  size_t operator()(const Chatik::SocketAddress& inAddress) const
  {
    return inAddress.GetHash();
  }
};
} // namespace std
