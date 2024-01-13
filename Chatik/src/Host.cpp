#include "Host.h"

#include "BaseSocket.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

using namespace Chatik;

Host::Host(bool isServer, bool useTCP /*=false*/)
  : mIsServer(isServer)
  , mUseTCP(useTCP)
  , mSocket(useTCP ? TCPSocket::CreateTCPSocket(SocketAddressFamily::INET)
                   : UDPSocket::CreateUDPSocket(SocketAddressFamily::INET))
{
  int res = mSocket->SetNonBlockingMode(true);
  assert(res == NO_ERROR);

  res = mSocket->Bind(
    SocketAddress(INADDR_ANY, mIsServer ? PORT_SERVER : PORT_CLIENT));
  assert(res == NO_ERROR);
}

Host::~Host()
{
  StopListening();

  for (auto& socket : mClientSockets) {
    delete socket;
    socket = nullptr;
  }

  delete mSocket;
  mSocket = nullptr;
}

bool
Host::StartListen()
{
  if (!mUseTCP) {
    mListenThread = std::thread([this]() {
      mIsListening.store(true, std::memory_order::relaxed);
      while (mIsListening.load(std::memory_order::relaxed)) {

        SocketAddress fromAddress;
        char buffer[1500];
        const int readByteCount =
          mSocket->Receive(buffer, sizeof(buffer), fromAddress);

        if (readByteCount <= SOCKET_ERROR) {
          if (readByteCount != -WSAESHUTDOWN) {
            std::cout << "Socket error: " << GetLastSocketError() << '\n';
          }

          mIsListening.store(false, std::memory_order::relaxed);
          break;
        }

        if (readByteCount > 0) {
          OnDataReceived(fromAddress, buffer, readByteCount);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });

    return true;
  } else {
    mListenThread = std::thread([this]() {
      mIsListening.store(true, std::memory_order::relaxed);
      while (mIsListening.load(std::memory_order::relaxed)) {
        if (mSocket->Listen() == NO_ERROR) {
          SocketAddress outFromAddress;
          if (BaseSocket* newSocket = mSocket->Accept(outFromAddress)) {
            std::cout << "New connection from " << outFromAddress.ToString()
                      << '\n';
            mClientSockets.push_back(newSocket);
          } else {
            const int errorNum = GetLastSocketError();
            if (errorNum != WSAEWOULDBLOCK && errorNum != EAGAIN) {
              std::cout << "Socket error: " << GetLastSocketError() << '\n';
              mIsListening.store(false, std::memory_order::relaxed);
              break;
            }
          }
        } else {
          std::cout << "Socket error: " << GetLastSocketError() << '\n';
          mIsListening.store(false, std::memory_order::relaxed);
          break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });
    return true;
  }

  return false;
}

bool
Host::StopListening()
{
  if (!mIsListening.load(std::memory_order::relaxed)) {
    return false;
  }

  if (mUseTCP) {
    shutdown(mSocket->GetSocket(), SD_BOTH);

    const int errorNum = GetLastSocketError();
    if (errorNum != NO_ERROR) {
      ReportSocketError("Host::StopListening");
    }
    assert(errorNum == NO_ERROR);
  }

  for (const auto& socket : mClientSockets) {
    if (mUseTCP) {
      shutdown(socket->GetSocket(), SD_BOTH);

      const int errorNum = GetLastSocketError();
      if (errorNum != NO_ERROR) {
        ReportSocketError("Host::StopListening");
      }
      assert(errorNum == NO_ERROR);
    }
  }

  if (mIsServer || !mUseTCP) {
    mIsListening.store(false, std::memory_order::relaxed);
  }

  if (mListenThread.joinable())
    mListenThread.join();

  return true;
}

bool
Host::IsValid() const
{
  return mSocket->IsValid();
}

int
Host::SendData(const char* data,
               int dataLen,
               const SocketAddress& toAddress) const
{
  return mSocket->Send(data, dataLen, toAddress);
}

bool
Chatik::Host::Connect(const SocketAddress& socketAddress) const
{
  int res = mSocket->SetNonBlockingMode(false);
  assert(res == NO_ERROR);

  const int connectRes = mSocket->Connect(socketAddress);

  res = mSocket->SetNonBlockingMode(true);
  assert(res == NO_ERROR);

  return connectRes == NO_ERROR;
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
