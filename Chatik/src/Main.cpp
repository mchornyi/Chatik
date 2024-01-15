#include "Chatik.h"
#include "ChatikCommon.h"
#include "SocketUtil.h"
#include "catch2/catch_session.hpp"

//#include "UnitTest++/UnitTest++.h"

using namespace std;

//int
//main()
//{
//  Chatik::SocketUtil::StaticInit();
//
//  cout << "I'm Chatik." << endl;
//
//  const int result = UnitTest::RunAllTests();
//
//  Chatik::SocketUtil::CleanUp();
//
//  return result;
//}

int main(int argc, char* argv[]) {
	// your setup ...
	Chatik::SocketUtil::StaticInit();

	cout << "I'm Chatik." << endl;

	int result = Catch::Session().run(argc, argv);

	// your clean-up...

	Chatik::SocketUtil::CleanUp();

	return result;
}