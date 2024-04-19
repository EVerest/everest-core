add_executable(test_iso15118_20 iso_20.cpp)

target_link_libraries(test_iso15118_20
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_iso15118_20)
