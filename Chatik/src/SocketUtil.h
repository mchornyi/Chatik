#pragma once

#include "ChatikCommon.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

namespace Chatik {

class SocketUtil
{
public:
  static bool StaticInit();
  static void CleanUp();

  static int Select(const vector<TCPSocketPtr>* inReadSet,
                    vector<TCPSocketPtr>* outReadSet,
                    const vector<TCPSocketPtr>* inWriteSet,
                    vector<TCPSocketPtr>* outWriteSet,
                    const vector<TCPSocketPtr>* inExceptSet,
                    vector<TCPSocketPtr>* outExceptSet);

private:
  static fd_set* FillSetFromVector(fd_set& outSet,
                                   const vector<TCPSocketPtr>* inSockets,
                                   int& ioNaxNfds);

  static void FillVectorFromSet(vector<TCPSocketPtr>* outSockets,
                                const vector<TCPSocketPtr>* inSockets,
                                const fd_set& inSet);
};
} // namespace Chatik
