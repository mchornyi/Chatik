#include "UnitTest++/UnitTest++.h"

#include "../src/SocketAddressFactory.h"

using namespace Chatik;

TEST(SocketAddressFactoryTest)
{
    const SocketAddressPtr address = SocketAddressFactory::CreateIPv4FromString("www.cplusplus.com");

    REQUIRE CHECK(address != nullptr);

    CHECK_EQUAL("54.39.7.252:0", address->ToString());

    delete address;
}