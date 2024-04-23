add_executable(test_v2gtp v2gtp.cpp)
target_link_libraries(test_v2gtp 
    PRIVATE
        cb_v2gtp
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(test_v2gtp)
