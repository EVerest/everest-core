include(Catch)

add_executable(test_authorization_setup authorization_setup.cpp)

target_link_libraries(test_authorization_setup
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_authorization_setup)

add_executable(test_authorization authorization.cpp)

target_link_libraries(test_authorization
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_authorization)

add_executable(test_service_discovery service_discovery.cpp)

target_link_libraries(test_service_discovery
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_service_discovery)

add_executable(test_service_detail service_detail.cpp)

target_link_libraries(test_service_detail
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_service_detail)

add_executable(test_service_selection service_selection.cpp)

target_link_libraries(test_service_selection
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_service_selection)

add_executable(test_dc_charge_parameter_discovery dc_charge_parameter_discovery.cpp)

target_link_libraries(test_dc_charge_parameter_discovery
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_dc_charge_parameter_discovery)

add_executable(test_schedule_exchange schedule_exchange.cpp)

target_link_libraries(test_schedule_exchange
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_schedule_exchange)

add_executable(test_dc_cable_check dc_cable_check.cpp)

target_link_libraries(test_dc_cable_check
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_dc_cable_check)

add_executable(test_dc_pre_charge dc_pre_charge.cpp)

target_link_libraries(test_dc_pre_charge
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_dc_pre_charge)

add_executable(test_power_delivery power_delivery.cpp)

target_link_libraries(test_power_delivery
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_power_delivery)

add_executable(test_dc_welding_detection dc_welding_detection.cpp)

target_link_libraries(test_dc_welding_detection
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_dc_welding_detection)

add_executable(test_session_stop session_stop.cpp)

target_link_libraries(test_session_stop
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_session_stop)
