cmake_minimum_required(VERSION 3.22)
project(modbus_server)

# remove CMake download extraction warning
# preserve the timestamps of files extracted from archives
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

option(BUILD_TESTS "Build tests" OFF)
if (BUILD_TESTS)
  option(TEST_COVERAGE "Enable test coverage" OFF)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(BUILD_TESTS)
  include(CTest)
  # GoogleTest using FetchContent
  include(FetchContent)
  FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  FetchContent_MakeAvailable(googletest)

  enable_testing()
endif()

add_subdirectory(libs)
add_subdirectory(examples)


if(BUILD_TESTS)
  if(TEST_COVERAGE)
    add_custom_target(coverage)
    # note that we do not exclude the tests from coverage as the headers would not be included in the report
    # this increases the coverage percentage but the coverage report itself is more accurate
    add_custom_command(TARGET coverage
        COMMAND gcovr --exclude-unreachable-branches -r . build/ -e examples/ -e build/_deps/ --xml-pretty -o build/coverage.xml
        COMMAND gcovr --exclude-unreachable-branches -r . build/ -e examples/ -e build/_deps/
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  endif()
endif()
