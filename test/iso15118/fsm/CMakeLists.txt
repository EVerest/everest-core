add_executable(test_d20_transitions d20_transitions.cpp)

target_sources(test_d20_transitions
    PRIVATE
        helper.cpp
)

target_link_libraries(test_d20_transitions
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_d20_transitions)
