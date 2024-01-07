#include "../src/SocketUtil.h"
#include "UnitTest++/UnitTest++.h"

SUITE(SocketUtilTest)
{
  TEST(FillSetFromVectorTest)
  {
    fd_set outSet{};
    const vector<Chatik::TCPSocketPtr>* inSockets = nullptr;
    int ioNaxNfds = 0;

    Chatik::SocketUtil::FillSetFromVector(outSet, inSockets, ioNaxNfds);

    CHECK_EQUAL(0, ioNaxNfds);
  }

  TEST(FillVectorFromSetTest)
  {
    vector<Chatik::TCPSocketPtr> outSockets;
    vector<Chatik::TCPSocketPtr> inSockets;
    const fd_set inSet{};

    Chatik::SocketUtil::FillVectorFromSet(&outSockets, &inSockets, inSet);

    CHECK_EQUAL(0, outSockets.size());
  }

  TEST(SocketAddressFactoryTest)
  {
    const Chatik::SocketAddress address = Chatik::SocketUtil::CreateIPv4FromString("www.cplusplus.com");

    REQUIRE CHECK(address.IsValid());

    CHECK_EQUAL("54.39.7.252:0", address.ToString());
  }
}
