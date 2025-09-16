cmake_minimum_required(VERSION 3.22)

project(IO VERSION 0.1
  DESCRIPTION "Event driven IO"
  LANGUAGES CXX C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH "Install path prefix" FORCE)
endif()

# Add ccache support
option(EVC_ENABLE_CCACHE "Enable ccache for faster compilation" OFF)
if(EVC_ENABLE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        message(STATUS "Using ccache: ${CCACHE_FOUND}")
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_FOUND})
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_FOUND})
        set(CMAKE_CUDA_COMPILER_LAUNCHER ${CCACHE_FOUND})
    else()
        message(WARNING "ccache not found but EVC_ENABLE_CCACHE was enabled")
    endif()
endif()

include(FetchContent)

# Only fetch mqttc if it hasn't been fetched already
if(NOT TARGET mqttc AND NOT DISABLE_EDM)
  FetchContent_Declare(
    mqttc
    GIT_REPOSITORY https://github.com/LiamBindle/MQTT-C.git
    GIT_TAG        v1.1.6
  )
  FetchContent_MakeAvailable(mqttc)
endif()

option(BUILD_DOC "Build documentation" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

add_subdirectory(src)

if (BUILD_TESTING)
    include(CTest)
    add_subdirectory(test)
endif()

if (BUILD_DOC)
  add_subdirectory(docs)
endif()

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
