set(TEST_TARGET_NAME ${PROJECT_NAME}_helpers_tests)

add_executable(${TEST_TARGET_NAME}
helpers_test.cpp
)

target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
        GTest::gmock_main
        GTest::gtest_main
        everest::staging::helpers
)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET_NAME})

ev_register_test_target(${TEST_TARGET_NAME})
