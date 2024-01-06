#pragma once

#include <thread>

#include "UDPSocket.h"

namespace Chatik {
class Host
{
public:
  Host();
  ~Host();

  bool StartListen();

  bool StopListening();

  bool IsValid() const { return mSocket->IsValid(); }

  bool IsListening() const
  {
    return mIsListening.load(std::memory_order::memory_order_relaxed);
  }

private:
  void OnDataReceived(const SocketAddress& fromAddress,
                      char* data,
                      int readByteCount) const;

private:
  UDPSocketPtr mSocket;
  std::thread mListenThread;
  std::atomic_bool mIsListening = false;
};
} // namespace Chatik
