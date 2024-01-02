#include "Chatik.h"
#include "ChatikCommon.h"
#include "SocketUtil.h"

#include "UnitTest++/UnitTest++.h"

using namespace std;

int main()
{
    Chatik::SocketUtil::StaticInit();

    cout << "I'm Chatik." << endl;

    const int result = UnitTest::RunAllTests();

    Chatik::SocketUtil::CleanUp();

    return result;
}
