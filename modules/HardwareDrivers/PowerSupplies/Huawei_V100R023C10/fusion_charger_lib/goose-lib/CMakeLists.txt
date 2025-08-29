cmake_minimum_required(VERSION 3.22)
project(goose-lib)

# remove CMake download extraction warning
# preserve the timestamps of files extracted from archives
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
  option(TEST_COVERAGE "Enable test coverage" OFF)

  include(FetchContent)
  include(CTest)
  enable_testing()

  # GoogleTest using FetchContent
  FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  FetchContent_MakeAvailable(googletest)

  if(TEST_COVERAGE)
    add_custom_target(coverage)
    add_custom_command(TARGET coverage
        COMMAND gcovr --exclude-unreachable-branches --gcov-exclude-directories ".*tests?.*" -r . build/ -e examples/ -e build/_deps/ -e tests --xml-pretty -o build/coverage.xml
        COMMAND gcovr --exclude-unreachable-branches --gcov-exclude-directories ".*tests?.*" -r . build/ -e examples/ -e build/_deps/ -e tests
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  endif()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(MACOSX TRUE)
endif()

add_subdirectory(libs)
add_subdirectory(examples)
