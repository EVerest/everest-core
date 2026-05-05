set(TEST_TARGET_NAME ${PROJECT_NAME}_transaction_handler_tests)
add_executable(${TEST_TARGET_NAME})

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})
get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ..
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
)

target_sources(${TEST_TARGET_NAME}
    PRIVATE
    ocpp_conversions_test.cpp
    transaction_handler_tests.cpp
    ../transaction_handler.cpp
)

target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
    everest::ocpp
    everest::framework
    GTest::gtest_main
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
