include(Catch)

add_executable(test_fsm_state_b state_b.cpp)

target_sources(test_fsm_state_b
    PRIVATE
        helper.cpp
)

target_link_libraries(test_fsm_state_b
    PRIVATE
        ieee2030::ieee2030
        Catch2::Catch2WithMain
)

catch_discover_tests(test_fsm_state_b)
