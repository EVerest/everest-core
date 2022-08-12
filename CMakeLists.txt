cmake_minimum_required(VERSION 3.11)

project(fsm VERSION 0.1
        DESCRIPTION "Finite State Machine library"
		LANGUAGES CXX
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

option(BUILD_EXAMPLES "enable building of examples" OFF)
option(FSM_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})

add_library(fsm INTERFACE)
add_library(fsm::fsm ALIAS fsm)

target_include_directories(fsm INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# build examples
if (BUILD_EXAMPLES)
    add_subdirectory(examples/light_switch)
endif()

if (FSM_INSTALL)
    install(
        TARGETS fsm
        EXPORT fsm-targets
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
    )

    evc_setup_package(
        NAME fsm
        EXPORT fsm-targets
        NAMESPACE fsm
    )
endif()
