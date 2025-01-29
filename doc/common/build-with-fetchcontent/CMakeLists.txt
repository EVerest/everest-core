cmake_minimum_required(VERSION 3.14)

project(builder
    VERSION 0.1
    DESCRIPTION "A example builder for libocpp and libtimer"
    LANGUAGES CXX
)

set(JSON_BUILD_TESTS OFF CACHE INTERNAL "Build JSON tests")
set(JSON_MultipleHeaders ON CACHE INTERNAL "Build JSON multi header config")
set(BUILD_TESTS OFF CACHE INTERNAL "Build tests")
set(BUILD_EXAMPLES OFF CACHE INTERNAL "Build examples")
set(BUILD_TZ_LIB ON CACHE INTERNAL "")
set(HAS_REMOTE_API 0 CACHE INTERNAL "")
set(USE_AUTOLOAD 0 CACHE INTERNAL "")
set(USE_SYSTEM_TZ_DB ON CACHE INTERNAL "")
set(JSON_VALIDATOR_INSTALL OFF CACHE INTERNAL "")
set(JSON_VALIDATOR_BUILD_TESTS OFF CACHE INTERNAL "")
set(JSON_VALIDATOR_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(JSON_VALIDATOR_BUILD_SHARED_LIBS ON CACHE INTERNAL "")

macro(find_date_library)
    message(STATUS "Using FetchContent for date library")
    set(EXTERNAL_DATE_NAME date)
    set(EXTERNAL_DATE_URL https://github.com/HowardHinnant/date.git)
    set(EXTERNAL_DATE_TAG v3.0.1)

    FetchContent_Declare(
        ${EXTERNAL_DATE_NAME}
        GIT_REPOSITORY ${EXTERNAL_DATE_URL}
        GIT_TAG ${EXTERNAL_DATE_TAG}
    )

    FetchContent_MakeAvailable(${EXTERNAL_DATE_NAME})
endmacro()

macro(find_everest_cmake)
    message(STATUS "Using FetchContent for everest-cmake")
    set(EXTERNAL_EVEREST_CMAKE_NAME everest-cmake)
    set(EXTERNAL_EVEREST_CMAKE_URL https://github.com/EVerest/everest-cmake.git)
    set(EXTERNAL_EVEREST_CMAKE_TAG v0.1.0)

    message(STATUS "Fetching everest cmake")
    FetchContent_Declare(
        ${EXTERNAL_EVEREST_CMAKE_NAME}
        GIT_REPOSITORY ${EXTERNAL_EVEREST_CMAKE_URL}
        GIT_TAG ${EXTERNAL_EVEREST_CMAKE_TAG}
    )

    FetchContent_MakeAvailable(${EXTERNAL_EVEREST_CMAKE_NAME})

    # Add the cmake directory to the module path
    # this is needed because some submodules (everest-timer and everest-log) still need to find the cmake module using "find_package"
    set(${EXTERNAL_EVEREST_CMAKE_NAME}_DIR ${CMAKE_BINARY_DIR}/_deps/${EXTERNAL_EVEREST_CMAKE_NAME}-src)
    _find_package(everest-cmake REQUIRED PATHS ${everest-cmake_DIR})
endmacro()

macro(find_nlohmann_json)
    message(STATUS "Using FetchContent for nlohmann_json library")
    set(EXTERNAL_JSON_NAME nlohmann_json)
    set(EXTERNAL_JSON_URL https://github.com/nlohmann/json.git)
    set(EXTERNAL_JSON_TAG v3.11.2)

    FetchContent_Declare(
        ${EXTERNAL_JSON_NAME}
        GIT_REPOSITORY ${EXTERNAL_JSON_URL}
        GIT_TAG ${EXTERNAL_JSON_TAG}
    )

    FetchContent_MakeAvailable(${EXTERNAL_JSON_NAME})
endmacro()

macro(find_nlohmann_json_schema_validator)
    message(STATUS "Using FetchContent for nlohmann_json_schema library")
    set(EXTERNAL_JSON_SCHEMA_NAME nlohmann_json_schema_validator)
    set(EXTERNAL_JSON_SCHEMA_URL https://github.com/pboettch/json-schema-validator)
    set(EXTERNAL_JSON_SCHEMA_TAG 2.2.0)

    FetchContent_Declare(
        ${EXTERNAL_JSON_SCHEMA_NAME}
        GIT_REPOSITORY ${EXTERNAL_JSON_SCHEMA_URL}
        GIT_TAG ${EXTERNAL_JSON_SCHEMA_TAG}
    )

    FetchContent_MakeAvailable(${EXTERNAL_JSON_SCHEMA_NAME})
endmacro()

macro(find_libwebsockets)
    message(STATUS "Using FetchContent for libwebsockets library")
    set(EXTERNAL_LIBWEBSOCKET_NAME libwebsockets)
    set(EXTERNAL_LIBWEBSOCKET_URL https://github.com/warmcat/libwebsockets.git)
    set(EXTERNAL_LIBWEBSOCKET_TAG v4.3.3)
    
    FetchContent_Declare(
        ${EXTERNAL_LIBWEBSOCKET_NAME}
        GIT_REPOSITORY ${EXTERNAL_LIBWEBSOCKET_URL}
        GIT_TAG ${EXTERNAL_LIBWEBSOCKET_TAG}
    )
    
    FetchContent_MakeAvailable(${EXTERNAL_LIBWEBSOCKET_NAME})
endmacro()

macro(find_websocketpp)
    message(STATUS "Using FetchContent for websocketpp library")
    set(EXTERNAL_WEBSOCKET_NAME websocketpp)
    set(EXTERNAL_WEBSOCKET_URL https://github.com/zaphoyd/websocketpp.git)
    set(EXTERNAL_WEBSOCKET_TAG 0.8.2)

    FetchContent_Declare(
        ${EXTERNAL_WEBSOCKET_NAME}
        GIT_REPOSITORY ${EXTERNAL_WEBSOCKET_URL}
        GIT_TAG ${EXTERNAL_WEBSOCKET_TAG}
    )

    FetchContent_MakeAvailable(${EXTERNAL_WEBSOCKET_NAME})
    add_library(websocketpp::websocketpp INTERFACE IMPORTED)
    set_target_properties(websocketpp::websocketpp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${websocketpp_SOURCE_DIR}")
endmacro()

macro(find_fsm)
    message(STATUS "Using FetchContent for fsm library")
    set(EXTERNAL_FSM_NAME fsm)
    set(EXTERNAL_FSM_URL https://github.com/EVerest/libfsm.git)
    set(EXTERNAL_FSM_TAG v0.2.0)

    FetchContent_Declare(
        ${EXTERNAL_FSM_NAME}
        GIT_REPOSITORY ${EXTERNAL_FSM_URL}
        GIT_TAG ${EXTERNAL_FSM_TAG}
    )

    FetchContent_MakeAvailable(${EXTERNAL_FSM_NAME})
endmacro()

macro(find_everest_timer)
    message(STATUS "Using FetchContent for everest-timer library")
    set(EXTERNAL_EVEREST_TIMER_NAME everest-timer)
    set(EXTERNAL_EVEREST_TIMER_URL https://github.com/EVerest/libtimer.git)
    set(EXTERNAL_EVEREST_TIMER_TAG v0.1.1)

    FetchContent_Declare(
        ${EXTERNAL_EVEREST_TIMER_NAME}
        GIT_REPOSITORY ${EXTERNAL_EVEREST_TIMER_URL}
        GIT_TAG ${EXTERNAL_EVEREST_TIMER_TAG}
    )

    FetchContent_MakeAvailable(${EXTERNAL_EVEREST_TIMER_NAME})
endmacro()

macro(find_everest_log)
    message(STATUS "Using FetchContent for everest-log library")
    set(EXTERNAL_EVEREST_LOG_NAME everest-log)
    set(EXTERNAL_EVEREST_LOG_URL https://github.com/EVerest/liblog.git)
    set(EXTERNAL_EVEREST_LOG_TAG v0.1.0)

    FetchContent_Declare(
        ${EXTERNAL_EVEREST_LOG_NAME}
        GIT_REPOSITORY ${EXTERNAL_EVEREST_LOG_URL}
        GIT_TAG ${EXTERNAL_EVEREST_LOG_TAG}
    )

    FetchContent_MakeAvailable(${EXTERNAL_EVEREST_LOG_NAME})
endmacro()

macro(find_package PACKAGE_NAME)
    message(STATUS "FIND PACKAGE ${PACKAGE_NAME}")

    if("${PACKAGE_NAME}" STREQUAL "date")
        find_date_library()
    elseif("${PACKAGE_NAME}" STREQUAL "everest-cmake")
        find_everest_cmake()
    elseif("${PACKAGE_NAME}" STREQUAL "nlohmann_json")
        find_nlohmann_json()
    elseif("${PACKAGE_NAME}" STREQUAL "nlohmann_json_schema_validator")
        find_nlohmann_json_schema_validator()
    elseif("${PACKAGE_NAME}" STREQUAL "libwebsockets")
        find_libwebsockets()
    elseif("${PACKAGE_NAME}" STREQUAL "websocketpp")
        find_websocketpp()
    elseif("${PACKAGE_NAME}" STREQUAL "fsm")
        find_fsm()
    elseif("${PACKAGE_NAME}" STREQUAL "everest-timer")
        find_everest_timer()
    elseif("${PACKAGE_NAME}" STREQUAL "everest-log")
        find_everest_log()
    else()
        message(STATUS "Using regular findpackage for ${PACKAGE_NAME}")
        _find_package(${ARGV})
    endif()
endmacro()

include(FetchContent)

set(EXTERNAL_OCPP_NAME libocpp)
set(EXTERNAL_OCPP_URL https://github.com/EVerest/libocpp.git)
set(EXTERNAL_OCPP_TAG main)
set(DISABLE_EDM ON CACHE INTERNAL "")
set(LIBOCPP16_BUILD_EXAMPLES ON CACHE INTERNAL "")

# Fetch libocpp
FetchContent_Declare(
    ${EXTERNAL_OCPP_NAME}
    GIT_REPOSITORY ${EXTERNAL_OCPP_URL}
    GIT_TAG ${EXTERNAL_OCPP_TAG}
)

# Find the libocpp package
FetchContent_MakeAvailable(${EXTERNAL_OCPP_NAME})
