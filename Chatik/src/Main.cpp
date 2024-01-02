#include "Chatik.h"
#include "ChatikCommon.h"

#include "UnitTest++/UnitTest++.h"

using namespace std;

TEST(Sanity)
{
    CHECK_EQUAL(1, 1);
}

int main()
{
    cout << "I'm Chatik." << endl;

    return UnitTest::RunAllTests();
}
