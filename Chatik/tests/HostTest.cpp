#include "../src/Host.h"
#include "../src/SocketUtil.h"

#include "catch2/catch_test_macros.hpp"

const char* kServerAddress = "127.0.0.1:5005";
const char* kClientAddress = "127.0.0.1:5006";
constexpr int kWaitForActionFromHost = 15000;

TEST_CASE("HostServerStartStopTest::UDP", "[.udp]")
{
  Chatik::Host hostServer(true);

  CHECK(true == hostServer.IsValid());
  CHECK(true == hostServer.StartListen());
  WAIT_FOR(hostServer.IsListening(), kWaitForActionFromHost);

  CHECK(true == hostServer.IsListening());
  CHECK(true == hostServer.ShutDown());

  WAIT_FOR(false == hostServer.IsListening(), kWaitForActionFromHost);
  CHECK(false == hostServer.IsListening());
}

TEST_CASE("HostClientStartStopTest::UDP", "[.udp]")
{
  Chatik::Host hostClient(false);

  CHECK(true == hostClient.IsValid());
  CHECK(true == hostClient.StartListen());
  WAIT_FOR(hostClient.IsListening(), kWaitForActionFromHost);

  CHECK(true == hostClient.IsListening());
  CHECK(true == hostClient.ShutDown());

  WAIT_FOR(false == hostClient.IsListening(), kWaitForActionFromHost);
  CHECK(false == hostClient.IsListening());
}

TEST_CASE("HostClientSendDataToServerTest::UDP", "[.udp]")
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

  WAIT_FOR(hostServer.IsListening(), kWaitForActionFromHost);
  WAIT_FOR(hostClient.IsListening(), kWaitForActionFromHost);

  constexpr char expectedData[] = "Hello, server!";

  const Chatik::SocketAddress serverAddress(
    Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

  const int sentDataLen =
    hostClient.SendData(expectedData, sizeof(expectedData), serverAddress);

  CHECK(sizeof(expectedData) == sentDataLen);

  WAIT_FOR(onServerDataArrivedTrigger.load(), kWaitForActionFromHost);

  CHECK(0 == strcmp(expectedData, actualData));
}

TEST_CASE("HostServerSendDataBackToClientTest::UDP", "[.udp]")
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

  WAIT_FOR(hostServer.IsListening(), kWaitForActionFromHost);
  WAIT_FOR(hostClient.IsListening(), kWaitForActionFromHost);

  const Chatik::SocketAddress serverAddress(
    Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

  constexpr char clientData[] = "Hello, server!";
  int sentDataLen =
    hostClient.SendData(clientData, sizeof(clientData), serverAddress);

  WAIT_FOR(onServerDataArrivedTrigger.load(), kWaitForActionFromHost);

  CHECK(0 == strcmp(clientData, actualData));

  constexpr char serverData[] = "Hello, client!";
	const Chatik::SocketAddress clientAddress(
		Chatik::SocketUtil::CreateIPv4FromString(kClientAddress));

  sentDataLen =
    hostServer.SendData(serverData, sizeof(serverData), clientAddress);

	CHECK(sizeof(serverData) == sentDataLen);

  WAIT_FOR(onClientDataArrivedTrigger.load(), kWaitForActionFromHost);

  CHECK(0 == strcmp(serverData, actualData));
}

TEST_CASE("HostServerStartStopTest::TCP", "[.tcp]")
{
  Chatik::Host hostServer(true, true);

  CHECK(true == hostServer.IsValid());
  CHECK(true == hostServer.StartListen());
  WAIT_FOR(hostServer.IsListening(), kWaitForActionFromHost);

  CHECK(true == hostServer.IsListening());
  CHECK(true == hostServer.ShutDown());

  WAIT_FOR(false == hostServer.IsListening(), kWaitForActionFromHost);

  CHECK(false == hostServer.IsListening());
}

TEST_CASE("HostClientConnectToServerAndShutDownTest::TCP", "[tcp]")
{
  Chatik::Host hostServer(true, true);

  hostServer.StartListen();
  WAIT_FOR(hostServer.IsListening(), kWaitForActionFromHost);

  Chatik::Host hostClient(false, true);

  REQUIRE(true == hostClient.IsValid());

  const Chatik::SocketAddress serverAddress(
    Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

  REQUIRE(true == hostClient.Connect(serverAddress));
	REQUIRE(true == hostClient.StartListen());
	WAIT_FOR(hostClient.IsListening(), kWaitForActionFromHost);
	REQUIRE(true == hostClient.IsListening());

  WAIT_FOR(hostServer.GetClientCount() > 0, kWaitForActionFromHost);
  REQUIRE(1 == hostServer.GetClientCount());

	REQUIRE(true == hostClient.ShutDown());

	WAIT_FOR(hostServer.GetClientCount() == 0, kWaitForActionFromHost);
	REQUIRE(0 == hostServer.GetClientCount());

	WAIT_FOR(false == hostClient.IsListening(), kWaitForActionFromHost);
	REQUIRE(false == hostClient.IsListening());
}

TEST_CASE("HostServertSendDataToClientTest::TCP", "[.tcp]")
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
  REQUIRE(true == hostServer.StartListen());

  Chatik::Host hostClient(false, true);
  hostClient.SetOnDataReceivedCallback(onClientDataReceivedCallback);

  const Chatik::SocketAddress serverAddress(
    Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

  REQUIRE(true == hostClient.Connect(serverAddress));

  WAIT_FOR(hostServer.GetClientCount() > 0, kWaitForActionFromHost);

  REQUIRE(1 == hostServer.GetClientCount());

  hostClient.StartListen();

  WAIT_FOR(hostClient.IsListening(), kWaitForActionFromHost);

  constexpr char expectedData[] = "Hello, client!";

  const Chatik::SocketAddress clientAddress{};

  const int sentDataLen =
    hostServer.SendData(expectedData, sizeof(expectedData), clientAddress);

  REQUIRE(sizeof(expectedData) == sentDataLen);

  WAIT_FOR(onClientDataArrivedTrigger.load(), kWaitForActionFromHost);

  REQUIRE(0 == strcmp(expectedData, actualData));
}
