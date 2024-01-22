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
  if (!mUseTCP) { // UDP
                  // Server/Client listens for incoming data
    ListenForIncomingData();
    return true;
  }

  // TCP Server
  if (mIsServer) {
    // Server listens for incoming connection
    ListenForNewConnections();
    ListenForIncomingDataFromClients();
    return true;
  } else { // TCP Client
           // Client listens for incoming data
    ListenForIncomingData();
    return true;
  }
}

bool
Host::StopListening()
{
  if (!mIsListening.load(std::memory_order::relaxed)) {
    if (mListenThread.joinable())
      mListenThread.join();
    return false;
  }

  if (mUseTCP && !mIsServer) {
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
Host::ShutDown()
{
  assert(!mIsShutDown && !mUseTCP);

  if (!mUseTCP) {
    return false;
  }

  if (mIsShutDown) {
    return true;
  }

  bool res = true;

  res &= mSocket->ShutDown();

  for (const auto& socket : mClientSockets) {
    res &= socket->ShutDown();
  }

  mIsShutDown = true;
  return res;
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
  if (!mUseTCP) {
    return mSocket->Send(data, dataLen, toAddress);
  } else {
    if (mIsServer) {
      int bytesSentCount = 0;
      for (const auto& socket : mClientSockets) {
        if (socket->GetSocket() == INVALID_SOCKET) {
          continue;
        }

        bytesSentCount += socket->Send(data, dataLen, toAddress);

        if (bytesSentCount < 0) {
          const int errorNum = GetLastSocketError();
          if (errorNum != WSAEWOULDBLOCK && errorNum != EAGAIN) {
            ReportSocketError("Host::SendData");
          }
          return -errorNum;
        }
      }

      return bytesSentCount;
    } else {
      return mSocket->Send(data, dataLen, toAddress);
    }
  }
}

bool
Host::Connect(const SocketAddress& socketAddress) const
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

void
Host::ListenForNewConnections()
{
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
}

void
Host::ListenForIncomingDataFromClients()
{
  mListenThreadClients = std::thread([this]() {
    mIsListening.store(true, std::memory_order::relaxed);
    while (mIsListening.load(std::memory_order::relaxed)) {

      for (const auto& cSocket : mClientSockets) {
        SocketAddress fromAddress{};
        char buffer[1500];
        const int readByteCount =
          cSocket->Receive(buffer, sizeof(buffer), fromAddress);

        if (readByteCount <= SOCKET_ERROR) {
          if (readByteCount != -WSAESHUTDOWN) {
            std::cout << "Socket error: " << GetLastSocketError() << '\n';
          }
        }

        if (readByteCount > 0) {
          OnDataReceived(fromAddress, buffer, readByteCount);
        }
      }

			// delete socket that were shutdown
      const auto removeFromIt = std::remove_if(mClientSockets.begin(),
                                               mClientSockets.end(),
                                               [](const BaseSocket* socket) {
                                                 if (socket->GetWasShutdown()) {
                                                   delete socket;
                                                   return true;
                                                 }
                                                 return false;
                                               });
      mClientSockets.erase(removeFromIt, mClientSockets.end());

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });
}

void
Host::ListenForIncomingData()
{
  mListenThread = std::thread([this]() {
    mIsListening.store(true, std::memory_order::relaxed);
    while (mIsListening.load(std::memory_order::relaxed)) {

      SocketAddress fromAddress{};
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
}
