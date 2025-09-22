set(TEST_TARGET_NAME ${PROJECT_NAME}_transaction_handler_tests)
add_executable(${TEST_TARGET_NAME} transaction_handler_tests.cpp)

set(MODULE_DIR 
    "${PROJECT_SOURCE_DIR}/modules/EVSE/OCPP201")

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ${MODULE_DIR}
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    everest::ocpp
    GTest::gtest_main
    )

target_sources(${TEST_TARGET_NAME} PRIVATE "../transaction_handler.cpp")

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
