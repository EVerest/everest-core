add_executable(test_app_handshake app_handshake.cpp)

target_link_libraries(test_app_handshake
    PRIVATE
        cb_v2g::DIN
        Catch2::Catch2WithMain
)

catch_discover_tests(test_app_handshake)