add_executable(charge_point charge_point.cpp)

target_link_libraries(charge_point
    PRIVATE
        Boost::thread
        Boost::program_options
        nlohmann_json::nlohmann_json
        nlohmann_json_schema_validator
        ocpp
        OpenSSL::SSL
        OpenSSL::Crypto
        SQLite::SQLite3
)

install(TARGETS charge_point
        RUNTIME)
set_property(TARGET ocpp PROPERTY POSITION_INDEPENDENT_CODE ON)
