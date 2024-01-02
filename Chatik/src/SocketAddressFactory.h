#pragma once

#include "SocketAddress.h"

namespace Chatik
{
class SocketAddressFactory
{
  public:
    static SocketAddressPtr CreateIPv4FromString(const string &inString);
};
} // namespace Chatik
