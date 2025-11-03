cmake_minimum_required(VERSION 3.15.7)

include(FetchContent)

project(fsm VERSION 0.1
        DESCRIPTION "Finite State Machine library"
		LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

add_library(Fsm INTERFACE)
add_library(Fsm::Fsm ALIAS Fsm)

target_include_directories(Fsm INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS Fsm EXPORT FsmTargets
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ TYPE INCLUDE)

install(EXPORT FsmTargets
    FILE FsmTargets.cmake
    NAMESPACE Fsm::
    DESTINATION lib/cmake/fsm
)


include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/FsmConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/fsm"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FsmConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/FsmConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/FsmConfigVersion.cmake
    DESTINATION lib/cmake/fsm
)

export(EXPORT FsmTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/FsmTargets.cmake"
)

# build examples
option(BUILD_EXAMPLES "enable building of examples" OFF)

if (BUILD_EXAMPLES)
    add_subdirectory(examples/light_switch)
endif()

option(BUILD_EXAMPLES_WITH_FREERTOS_SUPPORT "enable building of examples which support the FreeRTOS x86 layer" OFF)
if(BUILD_EXAMPLES_WITH_FREERTOS_SUPPORT)
    FetchContent_Declare(freertos
        GIT_REPOSITORY git@github.com:a-w50/freertos-posix-port.git
        GIT_TAG main
    )

    FetchContent_MakeAvailable(freertos)
endif()
