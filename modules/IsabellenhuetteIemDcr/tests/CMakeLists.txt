set(TEST_TARGET_NAME ${PROJECT_NAME}_isabellenhuette_iem_dcr_tests)

add_executable(${TEST_TARGET_NAME} isabellenhuette_iem_dcr_tests.cpp)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ${GENERATED_INCLUDE_DIR}
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
