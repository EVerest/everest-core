cmake_minimum_required(VERSION 3.22)
project(fusion-charge-dispenser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# remove CMake download extraction warning
# preserve the timestamps of files extracted from archives
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()


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

file(GLOB LIB_SOURCES lib/*.cpp)
add_library(fusion_charger_dispenser STATIC ${LIB_SOURCES})
target_include_directories(fusion_charger_dispenser PUBLIC include)
target_link_libraries(fusion_charger_dispenser PUBLIC fusion_charger_goose_driver fusion_charger_modbus_driver fusion_charger_modbus_extensions modbus-ssl Huawei::FusionCharger::LogInterface)

option(BUILD_ACCEPTANCE_TESTS "Build acceptance tests" OFF)
if(BUILD_TESTING)
  add_subdirectory(tests)

  if (BUILD_ACCEPTANCE_TESTS)
    add_subdirectory(user-acceptance-tests)
  endif()

endif()

add_subdirectory(power_stack_mock)


