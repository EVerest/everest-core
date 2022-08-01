# OCPP library
add_library(ocpp)
add_library(everest::ocpp ALIAS ocpp)
target_sources(ocpp
    PRIVATE
        ocpp1_6/charge_point.cpp
        ocpp1_6/charge_point_configuration.cpp
        ocpp1_6/charge_point_state_machine.cpp
        ocpp1_6/transaction.cpp
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

target_include_directories(ocpp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(ocpp
    PUBLIC
        everest::log
        everest::timer
        fsm::fsm
        websocketpp::websocketpp
    PRIVATE
        Boost::filesystem
        OpenSSL::SSL
        OpenSSL::Crypto
        SQLite::SQLite3
        Threads::Threads

        nlohmann_json::nlohmann_json
        nlohmann_json_schema_validator
        date::date-tz
)

# FIXME (aw): right now nlohmann_json and boost::optional don't compile
#             with gcc 10.x and C++11/14, so we need to publish the
#             C++17 standard
target_compile_features(ocpp PUBLIC cxx_std_17)
