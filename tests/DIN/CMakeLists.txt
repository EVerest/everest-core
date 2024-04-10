add_executable(test_session_setup session_setup.cpp)

target_link_libraries(test_session_setup
    PRIVATE
        cb_v2g::DIN
        Catch2::Catch2WithMain
)

catch_discover_tests(test_session_setup)


add_executable(test_service_discovery service_discovery.cpp)

target_link_libraries(test_service_discovery
        PRIVATE
        cb_v2g::DIN
        Catch2::Catch2WithMain
)

catch_discover_tests(test_service_discovery)
