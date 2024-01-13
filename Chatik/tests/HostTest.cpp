#include "../src/Host.h"
#include "../src/SocketUtil.h"
#include "UnitTest++/UnitTest++.h"

SUITE(HostTest)
{
  const char* kServerAddress = "127.0.0.1:5005";
  const char* kClientAddress = "127.0.0.1:5006";

  SUITE(UDP)
  {
    TEST(HostServerStartStopTest)
    {
      Chatik::Host host(true);

      CHECK_EQUAL(true, host.IsValid());
      CHECK_EQUAL(true, host.StartListen());
      WAIT_FOR(host.IsListening(), 200);

      CHECK_EQUAL(true, host.IsListening());
      CHECK_EQUAL(true, host.StopListening());
      CHECK_EQUAL(false, host.IsListening());
    }

    TEST(HostClientStartStopTest)
    {
      Chatik::Host host(false);

      CHECK_EQUAL(true, host.IsValid());
      CHECK_EQUAL(true, host.StartListen());
      WAIT_FOR(host.IsListening(), 200);

      CHECK_EQUAL(true, host.IsListening());
      CHECK_EQUAL(true, host.StopListening());
      CHECK_EQUAL(false, host.IsListening());
    }

    TEST(HostClientSendDataToServerTest)
    {
      char actualData[100] = {};

      std::atomic_bool onServerDataArrivedTrigger(false);

      auto onServerDataReceivedCallback =
        [&](const char* data,
            int dataLen,
            const Chatik::SocketAddress& fromAddress) {
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

      CHECK_EQUAL(sizeof(expectedData), sentDataLen);

      WAIT_FOR(onServerDataArrivedTrigger.load(), 200);

      CHECK_EQUAL(0, strcmp(expectedData, actualData));
    }

    TEST(HostServerSendDataToClientTest)
    {
      char actualData[100] = {};

      std::atomic_bool onServerDataArrivedTrigger(false);
      std::atomic_bool onClientDataArrivedTrigger(false);

      auto onServerDataReceivedCallback =
        [&](const char* data,
            int dataLen,
            const Chatik::SocketAddress& fromAddress) {
          assert(data && dataLen > 0);
          memcpy(actualData, data, dataLen);
          onServerDataArrivedTrigger = true;
        };

      auto onClientDataReceivedCallback =
        [&](const char* data,
            int dataLen,
            const Chatik::SocketAddress& fromAddress) {
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

      CHECK_EQUAL(0, strcmp(clientData, actualData));

      constexpr char expectedData[] = "Hello, client!";

      sentDataLen =
        hostServer.SendData(expectedData, sizeof(expectedData), serverAddress);

      WAIT_FOR(onClientDataArrivedTrigger.load(), 200);

      CHECK_EQUAL(0, strcmp(expectedData, actualData));
    }
  }

  SUITE(TCP)
  {
    TEST(HostServerStartStopTest)
    {
      Chatik::Host host(true, true);

      CHECK_EQUAL(true, host.IsValid());
      CHECK_EQUAL(true, host.StartListen());
      WAIT_FOR(host.IsListening(), 200);

      CHECK_EQUAL(true, host.IsListening());
      CHECK_EQUAL(true, host.StopListening());
      CHECK_EQUAL(false, host.IsListening());
    }

    TEST(HostClientConnectToServerTest)
    {
      Chatik::Host hostServer(true, true);

      CHECK_EQUAL(true, hostServer.IsValid());
      CHECK_EQUAL(true, hostServer.StartListen());
      WAIT_FOR(hostServer.IsListening(), 200);

      CHECK_EQUAL(true, hostServer.IsListening());

      Chatik::Host hostClient(false, true);

      CHECK_EQUAL(true, hostClient.IsValid());

      const Chatik::SocketAddress serverAddress(
        Chatik::SocketUtil::CreateIPv4FromString(kServerAddress));

      CHECK_EQUAL(true, hostClient.Connect(serverAddress));

      WAIT_FOR(hostServer.GetClientCount() > 0, 200);
      CHECK_EQUAL(1, hostServer.GetClientCount());

      CHECK_EQUAL(true, hostClient.Disconnect());

      CHECK_EQUAL(true, hostServer.StopListening());
      CHECK_EQUAL(false, hostServer.IsListening());
    }
  }
}
