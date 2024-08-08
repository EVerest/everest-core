include(Catch)

add_executable(test_logging logging.cpp)

target_link_libraries(test_logging
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_logging)
