#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "WinSock2.h"
#include "Windows.h"
#include "Ws2tcpip.h"
typedef int socklen_t;
// typedef char* receiveBufer_t;
#else
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// typedef void* receiveBufer_t;
typedef int SOCKET;
const int NO_ERROR = 0;
const int INVALID_SOCKET = -1;
const int WSAECONNRESET = ECONNRESET;
const int WSAEWOULDBLOCK = EAGAIN;
const int WSAESHUTDOWN = ESHUTDOWN;
const int SOCKET_ERROR = -1;
const int SD_BOTH = SHUT_RDWR;
#endif

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

enum SocketAddressFamily
{
  INET = AF_INET,
  INET6 = AF_INET6
};

#if !_WIN32
extern const char** __argv;
extern int __argc;

inline void
OutputDebugString(const char* inString)
{
  printf("%s", inString);
}
#endif

#pragma region LOG
inline string
GetCommandLineArg(int inIndex)
{
  if (inIndex < __argc) {
    return string(__argv[inIndex]);
  }

  return {};
}

inline string
Sprintf(const char* inFormat, ...)
{
  // not thread safe...
  static char temp[4096];

  va_list args;
  va_start(args, inFormat);

#if _WIN32
  _vsnprintf_s(temp, 4096, 4096, inFormat, args);
#else
  vsnprintf(temp, 4096, inFormat, args);
#endif
  return string(temp);
}

// void Log( const char* inFormat )
// {
// 	OutputDebugString( inFormat );
// 	OutputDebugString( "\n" );
// }

inline void
Log(const char* inFormat, ...)
{
  // not thread safe...
  static char temp[4096];

  va_list args;
  va_start(args, inFormat);

#if _WIN32
  _vsnprintf_s(temp, 4096, 4096, inFormat, args);
#else
  vsnprintf(temp, 4096, inFormat, args);
#endif
  OutputDebugString(temp);
  OutputDebugString("\n");
}

#define LOG(...) Log(__VA_ARGS__)

#pragma endregion LOG

#pragma region ERROR
inline int
GetLastSocketError()
{
#if _WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}

inline void
ReportSocketError(const char* inOperationDesc)
{
	const int errorNum = GetLastSocketError();

#if _WIN32
  LPVOID lpMsgBuf;
  
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorNum,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf,
                0,
                NULL);
	LOG("Error %s: %d- %s", inOperationDesc, errorNum, lpMsgBuf);
#else
	char msg[64];
	strerror_r(errorNum, msg, sizeof(msg));
	LOG("Error %s: %d- %s", inOperationDesc, errorNum, strerror(errorNum));
#endif
}
#pragma endregion ERROR

#define WAIT_FOR(exp, timeout)                                                 \
  {                                                                            \
    int i = 0;                                                                 \
    while (!(exp)) {                                                           \
      if (i++ > timeout)                                                       \
        break;                                                                 \
      std::this_thread::sleep_for(std::chrono::milliseconds(1));               \
    }                                                                          \
  }
