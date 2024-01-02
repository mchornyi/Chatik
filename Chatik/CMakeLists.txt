# CMakeList.txt : CMake project for Chatik, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Chatik")

add_subdirectory(${PROJECT_SOURCE_DIR}/unittest-cpp)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB TESTS_SOURCES ${PROJECT_SOURCE_DIR}/tests/*.cpp ${PROJECT_SOURCE_DIR}/tests/*.h)

# Add source to this project's executable.
add_executable(Chatik ${SOURCES} ${TESTS_SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chatik PROPERTY CXX_STANDARD 20)
endif()

if(WIN32)
  target_link_libraries(Chatik ws2_32)
endif()

target_link_libraries(Chatik UnitTest++)

# TODO: Add tests and install targets if needed.
