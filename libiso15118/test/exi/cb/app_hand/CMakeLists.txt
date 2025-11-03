add_executable(test_app_hand app_hand.cpp)

target_link_libraries(test_app_hand
    PRIVATE
        iso15118
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(test_app_hand)
