#include "Host.h"

#include <thread>

using namespace Chatik;

static const int PORT = 5005;

Host::Host()
  : mSocket(UDPSocket::CreateUDPSocket(SocketAddressFamily::INET))
{
  int res = mSocket->SetNonBlockingMode(true);
  assert(res == NO_ERROR);

  res = mSocket->Bind(SocketAddress(INADDR_ANY, PORT));
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
    mIsListening.store(true, std::memory_order::memory_order_relaxed);
    while (mIsListening.load(std::memory_order::memory_order_relaxed)) {

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
  if (!mIsListening.load(std::memory_order::memory_order_relaxed)) {
    return false;
  }

  mIsListening.store(false, std::memory_order::memory_order_relaxed);

  mListenThread.join();

  return true;
}

void
Host::OnDataReceived(const SocketAddress& fromAddress,
                     char* data,
                     int readByteCount) const
{
  assert(data && readByteCount > 0);

  std::cout << "Received " << readByteCount << " bytes from "
            << fromAddress.ToString() << '\n';
}
