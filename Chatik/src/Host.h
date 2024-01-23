#pragma once

#include "SocketAddress.h"

#include <functional>
#include <thread>

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

	bool ShutDown();

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

  size_t GetClientCount() const { return mClientSockets.size(); }
  bool Connect(const SocketAddress& socketAddress) const;

private:
	bool StopListening();

  void OnDataReceived(const SocketAddress& fromAddress,
                      const char* data,
                      int readByteCount) const;

	void ListenForNewConnections();
	void ListenForIncomingDataFromClients();
	void ListenForIncomingData();

private:
  bool mIsServer = false;
  bool mUseTCP = false;
	bool mIsShutDown = false;
  BaseSocket* mSocket;
  std::thread mListenThread;
	std::thread mListenThreadClients;
  std::atomic_bool mIsListening = false;
  OnDataReceiveCallback mOnDataReceivedCallback;
  std::vector<BaseSocket*> mClientSockets;
	std::vector<BaseSocket*> mWasShutDownClientSockets;
	Spinlock mSpinLock;
};
} // namespace Chatik
