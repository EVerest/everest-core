cmake_minimum_required(VERSION 3.16)
project(huawei-fusion-charger)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(BUILD_TESTS)
  include(CTest)
endif()


if(DEFINED MODULE_NAME)
  set(BUILDING_IN_EVEREST ON)
else()
  set(BUILDING_IN_EVEREST OFF)
endif()


if(NOT BUILDING_IN_EVEREST)
  message(STATUS "Not building in Everest environment, fetching MQTT-C")
  include(FetchContent)
  FetchContent_Declare(
    MQTT-C
    GIT_REPOSITORY https://github.com/LiamBindle/MQTT-C
    GIT_TAG        v1.1.6
  )
  FetchContent_MakeAvailable(MQTT-C)
endif()

add_subdirectory(fusion-charger-dispenser-library)
add_subdirectory(goose-lib)
add_subdirectory(huawei-fusioncharge-driver)
add_subdirectory(modbus-server)
add_subdirectory(log)
