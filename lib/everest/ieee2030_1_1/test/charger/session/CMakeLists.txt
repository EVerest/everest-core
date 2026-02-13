include(Catch)

add_executable(test_callback test_callback.cpp)

target_link_libraries(test_callback
    PRIVATE
        ieee2030::ieee2030
        Catch2::Catch2WithMain
)

catch_discover_tests(test_callback)
