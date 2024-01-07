#include "../src/Host.h"
#include "../src/SocketUtil.h"
#include "UnitTest++/UnitTest++.h"

static std::atomic_bool onDataArrived;

SUITE(HostTest)
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
    constexpr char expectedData[] = "Hello, world!";
    char actualData[100] = {};

    onDataArrived = false;

    auto onDataReceivedCallback =
      [&](const char* data,
          int dataLen,
          const Chatik::SocketAddress& fromAddress) {
        assert(data && dataLen > 0);
        memcpy(actualData, data, dataLen);

        onDataArrived = true;
      };

    Chatik::Host hostServer(true);
    hostServer.SetOnDataReceivedCallback(onDataReceivedCallback);

    hostServer.StartListen();

    Chatik::Host hostClient(false);
    hostClient.StartListen();

    WAIT_FOR(hostServer.IsListening(), 200);
    WAIT_FOR(hostClient.IsListening(), 200);

    const Chatik::SocketAddress address(
      Chatik::SocketUtil::CreateIPv4FromString("127.0.0.1:5005"));

    const int sentDataLen =
      hostClient.SendData(expectedData, sizeof(expectedData), address);

    CHECK_EQUAL(sizeof(expectedData), sentDataLen);

    WAIT_FOR(onDataArrived.load(), 200);

    CHECK_EQUAL(0, strcmp(expectedData, actualData));
  }
}
