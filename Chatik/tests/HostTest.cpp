#include "../src/Host.h"
#include "UnitTest++/UnitTest++.h"

SUITE(HostTest)
{
  TEST(HostStartStopTest)
  {
    Chatik::Host host;

    CHECK_EQUAL(true, host.IsValid());
    CHECK_EQUAL(true, host.StartListen());
    WAIT_FOR(host.IsListening(), 200);

    CHECK_EQUAL(true, host.IsListening());
    CHECK_EQUAL(true, host.StopListening());
    CHECK_EQUAL(false , host.IsListening());
  }
}
