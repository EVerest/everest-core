set(TEST_TARGET_NAME ${PROJECT_NAME}_fsm_state_allocator_tests)

add_executable(${TEST_TARGET_NAME} state_allocator.cpp)
target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
        fsm::fsm
        Catch2::Catch2WithMain
)

catch_discover_tests(${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
