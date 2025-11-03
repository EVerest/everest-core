include(Catch)

add_executable(test_feedback feedback.cpp)

target_link_libraries(test_feedback
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

catch_discover_tests(test_feedback)
