add_executable(test_app_handshake app_handshake.cpp)

target_link_libraries(test_app_handshake
    PRIVATE
        cbv2g::din
        Catch2::Catch2WithMain
)

catch_discover_tests(test_app_handshake)