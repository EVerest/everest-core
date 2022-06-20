set(HEADERS
    "${PROJECT_SOURCE_DIR}/include/ocpp.hpp"
)

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# OCPP library
add_library(ocpp STATIC)
target_sources(ocpp
    PRIVATE
        ocpp1_6/charge_point.cpp
        ocpp1_6/charge_point_configuration.cpp
        ocpp1_6/charge_point_state_machine.cpp
        ocpp1_6/charging_session.cpp
        ocpp1_6/enums.cpp
        ocpp1_6/message_queue.cpp
        ocpp1_6/ocpp_types.cpp
        ocpp1_6/pki_handler.cpp
        ocpp1_6/schemas.cpp
        ocpp1_6/types.cpp
        ocpp1_6/types_internal.cpp
)

add_subdirectory(ocpp1_6/messages)
add_subdirectory(ocpp1_6/websocket)

target_include_directories(ocpp PUBLIC ${INCLUDE_DIR} ${websocketpp_SOURCE_DIR})
target_link_libraries(ocpp
    PRIVATE
        Boost::filesystem
        Boost::system
        Boost::thread
        date::date
        date::date-tz
        everest::log
        everest::timer
        fsm::fsm
        nlohmann_json::nlohmann_json
        nlohmann_json_schema_validator::nlohmann_json_schema_validator
        OpenSSL::SSL
        SQLite::SQLite3
)

# this is important to allow our headers to find the everest::log headers
target_link_libraries(ocpp
    INTERFACE
        everest::log)

# this is important to allow our headers to find the everest::timer headers
target_link_libraries(ocpp
INTERFACE
    everest::timer)

# this is important to allow our headers to find the libfsm headers
target_link_libraries(ocpp
    INTERFACE
        fsm::fsm)
# this is important to allow our headers to find the nlohmann_* headers
target_link_libraries(ocpp
INTERFACE
    nlohmann_json_schema_validator::nlohmann_json_schema_validator)

# needs c++ 14
target_compile_features(ocpp PUBLIC cxx_std_14)

source_group(
    TREE "${INCLUDE_DIR}"
    PREFIX "Headers"
    FILES ${HEADERS})
