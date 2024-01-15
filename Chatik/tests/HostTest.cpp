#include "../src/Host.h"
#include "../src/SocketUtil.h"

#include "catch2/catch_test_macros.hpp"

const char* kServerAddress = "127.0.0.1:5005";
const char* kClientAddress = "127.0.0.1:5006";

TEST_CASE("HostServerStartStopTest::UDP", "[udp]")
{
  Chatik::Host host(true);

  CHECK(true == host.IsValid());
  CHECK(true == host.StartListen());
  WAIT_FOR(host.IsListening(), 200);

  CHECK(true == host.IsListening());
  CHECK(true == host.StopListening());
  CHECK(false == host.IsListening());
}

TEST_CASE("HostClientStartStopTest::UDP", "[udp]")
{
  Chatik::Host host(false);

  CHECK(true == host.IsValid());
  CHECK(true == host.StartListen());
  WAIT_FOR(host.IsListening(), 200);

  CHECK(true == host.IsListening());
  CHECK(true == host.StopListening());
  CHECK(false == host.IsListening());
}

TEST_CASE("HostClientSendDataToServerTest::UDP", "[udp]")
{
  char actualData[100] = {};

  std::atomic_bool onServerDataArrivedTrigger(false);

  auto onServerDataReceivedCallback =
    [&](
      const char* data, int dataLen, const Chatik::SocketAddress& fromAddress) {
      assert(data && dataLen > 0);
      memcpy(actualData, data, dataLen);
      onServerDataArrivedTrigger = true;
    };

  Chatik::Host hostServer(true);
  hostServer.SetOnDataReceivedCallback(onServerDataReceivedCallback);

  hostServer.StartListen();

  Chatik::Host hostClient(false);
  hostClient.StartListen();

  WAIT_FOR(hostServer.IsListening(), 200);
  WAIT_FOR(hostClient.IsListening(), 200);

  constexpr char expectedData[] = "Hello, server!";

  const Chatik::SocketAddress serverAddress(
    Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

  const int sentDataLen =
    hostClient.SendData(expectedData, sizeof(expectedData), serverAddress);

  CHECK(sizeof(expectedData) == sentDataLen);

  WAIT_FOR(onServerDataArrivedTrigger.load(), 200);

  CHECK(0 == strcmp(expectedData, actualData));
}

TEST_CASE("HostServerSendDataToClientTest::UDP", "[udp]")
{
  char actualData[100] = {};

  std::atomic_bool onServerDataArrivedTrigger(false);
  std::atomic_bool onClientDataArrivedTrigger(false);

  auto onServerDataReceivedCallback =
    [&](
      const char* data, int dataLen, const Chatik::SocketAddress& fromAddress) {
      assert(data && dataLen > 0);
      memcpy(actualData, data, dataLen);
      onServerDataArrivedTrigger = true;
    };

  auto onClientDataReceivedCallback =
    [&](
      const char* data, int dataLen, const Chatik::SocketAddress& fromAddress) {
      assert(data && dataLen > 0);
      memcpy(actualData, data, dataLen);
      onClientDataArrivedTrigger = true;
    };

  Chatik::Host hostServer(true);
  hostServer.SetOnDataReceivedCallback(onServerDataReceivedCallback);
  hostServer.StartListen();

  Chatik::Host hostClient(false);
  hostClient.SetOnDataReceivedCallback(onClientDataReceivedCallback);
  hostClient.StartListen();

  WAIT_FOR(hostServer.IsListening(), 200);
  WAIT_FOR(hostClient.IsListening(), 200);

  const Chatik::SocketAddress serverAddress(
    Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

  constexpr char clientData[] = "Hello, server!";
  int sentDataLen =
    hostClient.SendData(clientData, sizeof(clientData), serverAddress);

  WAIT_FOR(onServerDataArrivedTrigger.load(), 200);

  CHECK(0 == strcmp(clientData, actualData));

  constexpr char expectedData[] = "Hello, client!";

  sentDataLen =
    hostServer.SendData(expectedData, sizeof(expectedData), serverAddress);

  WAIT_FOR(onClientDataArrivedTrigger.load(), 200);

  CHECK(0 == strcmp(expectedData, actualData));
}

TEST_CASE("HostServerStartStopTest::TCP", "[tcp]")
{
  Chatik::Host host(true, true);

  CHECK(true == host.IsValid());
  CHECK(true == host.StartListen());
  WAIT_FOR(host.IsListening(), 200);

  CHECK(true == host.IsListening());
  CHECK(true == host.StopListening());
  CHECK(false == host.IsListening());
}

TEST_CASE("HostClientConnectToServerTest::TCP", "[tcp]")
{
  Chatik::Host hostServer(true, true);

  hostServer.StartListen();
  WAIT_FOR(hostServer.IsListening(), 200);

  Chatik::Host hostClient(false, true);

  CHECK(true == hostClient.IsValid());

  const Chatik::SocketAddress serverAddress(
    Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

  CHECK(true == hostClient.Connect(serverAddress));

  WAIT_FOR(hostServer.GetClientCount() > 0, 200);
  CHECK(1 == hostServer.GetClientCount());
}

TEST_CASE("HostServertSendDataToClientTest::TCP", "[tcp][.]")
{
  char actualData[100] = {};

  std::atomic_bool onClientDataArrivedTrigger(false);

  auto onClientDataReceivedCallback =
    [&](
      const char* data, int dataLen, const Chatik::SocketAddress& fromAddress) {
      assert(data && dataLen > 0);
      memcpy(actualData, data, dataLen);
      onClientDataArrivedTrigger = true;
    };

  Chatik::Host hostServer(true, true);
  hostServer.StartListen();

  Chatik::Host hostClient(false, true);
  hostClient.SetOnDataReceivedCallback(onClientDataReceivedCallback);

  const Chatik::SocketAddress serverAddress(
    Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

  const int res = hostClient.Connect(serverAddress);

  WAIT_FOR(hostServer.GetClientCount() > 0, 200);

  hostClient.StartListen();

  WAIT_FOR(hostClient.IsListening(), 200);

  constexpr char expectedData[] = "Hello, client!";

  const Chatik::SocketAddress clientAddress{};

  const int sentDataLen =
    hostServer.SendData(expectedData, sizeof(expectedData), clientAddress);

  CHECK(sizeof(expectedData) == sentDataLen);

  WAIT_FOR(onClientDataArrivedTrigger.load(), 200);

  CHECK(0 == strcmp(expectedData, actualData));
}
