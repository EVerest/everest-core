include(Catch)

add_executable(test_timeout test_timeout.cpp)

target_link_libraries(test_timeout
    PRIVATE
        ieee2030::ieee2030
        Catch2::Catch2WithMain
)

catch_discover_tests(test_timeout)
