cmake_minimum_required(VERSION 3.16)
project(huawei-fusion-charger)

if(DEFINED MODULE_NAME)
  set(BUILDING_IN_EVEREST ON)
else()
  set(BUILDING_IN_EVEREST OFF)
endif()

if(BUILDING_IN_EVEREST)
  set(FUSION_CHARGER_LIB_BUILD_TESTS ${EVEREST_CORE_BUILD_TESTING})
else()
  option(BUILD_TESTING "Build tests" OFF)
  set(FUSION_CHARGER_LIB_BUILD_TESTS ${BUILD_TESTING})
endif()

if(NOT BUILDING_IN_EVEREST)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
  endif()

  include(FetchContent)
  FetchContent_Declare(
    MQTT-C
    GIT_REPOSITORY https://github.com/LiamBindle/MQTT-C
    GIT_TAG        v1.1.6
  )
  FetchContent_MakeAvailable(MQTT-C)

  if(FUSION_CHARGER_LIB_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)
  endif()

  if(FUSION_CHARGER_LIB_BUILD_TESTS)
    include(CTest)
  endif()
endif()

add_subdirectory(fusion-charger-dispenser-library)
add_subdirectory(goose-lib)
add_subdirectory(huawei-fusioncharge-driver)
add_subdirectory(modbus-server)
add_subdirectory(log)
