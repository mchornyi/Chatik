#pragma once

#include "SocketAddress.h"

#include <thread>
#include <functional>

namespace Chatik {
static constexpr int PORT_SERVER = 5005;
static constexpr int PORT_CLIENT = 5006;

class BaseSocket;

using OnDataReceiveCallback = std::function<
  void(const char* data, int dataLen, const SocketAddress& fromAddress)>;

class Host
{
public:
  Host(bool isServer, bool useTCP = false);
  ~Host();

  bool StartListen();

  bool StopListening();

  bool IsValid() const;

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
  bool mIsServer = false;
  BaseSocket* mSocket;
  std::thread mListenThread;
  std::atomic_bool mIsListening = false;
  OnDataReceiveCallback mOnDataReceivedCallback;  
};
} // namespace Chatik
