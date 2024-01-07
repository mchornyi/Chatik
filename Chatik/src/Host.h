#pragma once

#include <functional>

#include "UDPSocket.h"
#include <thread>

namespace Chatik {
static constexpr int PORT_SERVER = 5005;
static constexpr int PORT_CLIENT = 5006;

using OnDataReceiveCallback = std::function<
  void(const char* data, int dataLen, const SocketAddress& fromAddress)>;

class Host
{
public:
  Host(bool isServer);
  ~Host();

  bool StartListen();

  bool StopListening();

  bool IsValid() const { return mSocket->IsValid(); }

  bool IsListening() const
  {
    return mIsListening.load(std::memory_order::relaxed);
  }

  int SendData(const char* data,
               int dataLen,
               const SocketAddress& toAddress) const;

  void SetOnDataReceivedCallback(const OnDataReceiveCallback& callback)
  {
    mOnDataReceivedCallback = callback;
  }

private:
  void OnDataReceived(const SocketAddress& fromAddress,
                      const char* data,
                      int readByteCount) const;

private:
  UDPSocketPtr mSocket;
  std::thread mListenThread;
  std::atomic_bool mIsListening = false;

  OnDataReceiveCallback mOnDataReceivedCallback;

  bool mIsServer = false;
};
} // namespace Chatik
