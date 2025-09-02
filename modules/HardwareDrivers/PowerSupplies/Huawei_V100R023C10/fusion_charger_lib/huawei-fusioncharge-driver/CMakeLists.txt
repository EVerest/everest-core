cmake_minimum_required(VERSION 3.22)
project(fusioncharge-driver)

# remove CMake download extraction warning
# preserve the timestamps of files extracted from archives
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
  include(CTest)
  enable_testing()

  include(FetchContent)
  # GoogleTest using FetchContent
  FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  FetchContent_MakeAvailable(googletest)
endif()

add_subdirectory(libs)
add_subdirectory(examples)
