#include "Host.h"

using namespace Chatik;

Host::Host(bool isServer)
  : mSocket(UDPSocket::CreateUDPSocket(SocketAddressFamily::INET))
  , mIsServer(isServer)
{
  int res = mSocket->SetNonBlockingMode(true);
  assert(res == NO_ERROR);

  res = mSocket->Bind(SocketAddress(INADDR_ANY, isServer ? PORT_SERVER : PORT_CLIENT));
  assert(res == NO_ERROR);
}

Host::~Host()
{
  StopListening();
  delete mSocket;
  mSocket = nullptr;
}

bool
Host::StartListen()
{
  mListenThread = std::thread([this]() {
    mIsListening.store(true, std::memory_order::relaxed);
    while (mIsListening.load(std::memory_order::relaxed)) {

      SocketAddress fromAddress;
      char buffer[1500];
      const int readByteCount =
        mSocket->ReceiveFrom(buffer, sizeof(buffer), fromAddress);

      if (readByteCount > 0) {
        OnDataReceived(fromAddress, buffer, readByteCount);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  return true;
}

bool
Host::StopListening()
{
  if (!mIsListening.load(std::memory_order::relaxed)) {
    return false;
  }

  mIsListening.store(false, std::memory_order::relaxed);

  mListenThread.join();

  return true;
}

int Host::SendData(const char* data, int dataLen, const SocketAddress& toAddress) const
{
  return mSocket->SendTo(data, dataLen, toAddress);
}

void
Host::OnDataReceived(const SocketAddress& fromAddress,
                     const char* data,
                     int readByteCount) const
{
  assert(data && readByteCount > 0);

  // std::cout << "Received " << readByteCount << " bytes from "
  //           << fromAddress.ToString() << '\n';

  mOnDataReceivedCallback(data, readByteCount, fromAddress);
}
