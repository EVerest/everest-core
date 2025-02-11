set(TEST_TARGET_NAME ${PROJECT_NAME}_API_tests)
add_executable(${TEST_TARGET_NAME})

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

target_include_directories(${TEST_TARGET_NAME} PRIVATE
    . .. ../../../tests/include
)

target_sources(${TEST_TARGET_NAME} PRIVATE
    StartupMonitor_test.cpp
    ../StartupMonitor.cpp
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gtest_main
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
