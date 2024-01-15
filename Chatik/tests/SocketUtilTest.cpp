#include "../src/SocketUtil.h"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("SocketUtilTest::FillSetFromVectorTest")
{
	fd_set outSet{};
	const vector<Chatik::TCPSocketPtr>* inSockets = nullptr;
	int ioNaxNfds = 0;

	Chatik::SocketUtil::FillSetFromVector(outSet, inSockets, ioNaxNfds);

	CHECK(0 == ioNaxNfds);
}

TEST_CASE("SocketUtilTest::FillVectorFromSetTest")
{
	vector<Chatik::TCPSocketPtr> outSockets;
	vector<Chatik::TCPSocketPtr> inSockets;
	const fd_set inSet{};

	Chatik::SocketUtil::FillVectorFromSet(&outSockets, &inSockets, inSet);

	CHECK(0 == outSockets.size());
}

TEST_CASE("SocketUtilTest::SocketAddressFactoryTest")
{
	const Chatik::SocketAddress address = Chatik::SocketUtil::CreateIPv4FromString("www.cplusplus.com");

	REQUIRE(address.IsValid());

	CHECK("54.39.7.252:0" == address.ToString());
}
