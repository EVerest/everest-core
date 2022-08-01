add_executable(charge_point charge_point.cpp)

target_link_libraries(charge_point
    PRIVATE
        Boost::thread
        Boost::program_options
        nlohmann_json::nlohmann_json
        ocpp
)

install(TARGETS charge_point
        RUNTIME)
