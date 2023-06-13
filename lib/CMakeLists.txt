# OCPP library
add_library(ocpp)
add_library(everest::ocpp ALIAS ocpp)
target_sources(ocpp
    PRIVATE
        ocpp/common/call_types.cpp
        ocpp/common/charging_station_base.cpp
        ocpp/common/message_queue.cpp
        ocpp/common/ocpp_logging.cpp
        ocpp/common/pki_handler.cpp
        ocpp/common/schemas.cpp
        ocpp/common/types.cpp
        ocpp/common/utils.cpp
        ocpp/v16/charge_point.cpp
        ocpp/v16/database_handler.cpp
        ocpp/v16/charge_point_impl.cpp
        ocpp/v16/smart_charging.cpp
        ocpp/v16/charge_point_configuration.cpp
        ocpp/v16/charge_point_state_machine.cpp
        ocpp/v16/transaction.cpp
        ocpp/v16/enums.cpp
        ocpp/v16/ocpp_types.cpp
        ocpp/v16/types.cpp
        ocpp/v201/charge_point.cpp
        ocpp/v201/database_handler.cpp
        ocpp/v201/device_model_management.cpp
        ocpp/v201/device_model_management_base.cpp
        ocpp/v201/transaction.cpp
        ocpp/v201/enums.cpp
        ocpp/v201/ocpp_types.cpp
        ocpp/v201/types.cpp
        ocpp/v201/connector.cpp
        ocpp/v201/evse.cpp
        ocpp/v201/utils.cpp
)

add_subdirectory(ocpp/common/websocket)
add_subdirectory(ocpp/v16/messages)
add_subdirectory(ocpp/v201/messages)


target_include_directories(ocpp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(ocpp
    PUBLIC
        everest::log
        everest::timer
        websocketpp::websocketpp
    PRIVATE
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
