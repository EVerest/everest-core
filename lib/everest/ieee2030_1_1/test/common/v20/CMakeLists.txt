include(Catch)

add_executable(test_event_queue test_event_queue.cpp)

target_link_libraries(test_event_queue
    PRIVATE
        ieee2030::ieee2030
        Catch2::Catch2WithMain
)

catch_discover_tests(test_event_queue)
