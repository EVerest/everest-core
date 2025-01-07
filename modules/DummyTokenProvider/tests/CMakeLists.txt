set(TEST_TARGET_NAME ${PROJECT_NAME}_dummy_token_provider_tests)

set(DUMMY_TOKEN_PROVIDER_MODULE_DIR ${PROJECT_SOURCE_DIR}/modules/DummyTokenProvider)

add_executable(${TEST_TARGET_NAME}
    dummy_token_provider_tests.cpp
    ${DUMMY_TOKEN_PROVIDER_MODULE_DIR}/main/auth_token_providerImpl.cpp
    ${DUMMY_TOKEN_PROVIDER_MODULE_DIR}/DummyTokenProvider.cpp
)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)
get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
set(DUMMY_TOKEN_PROVIDER_MODULE_LOADER_DIR ${GENERATED_OUTPUT_DIR}/modules/${MODULE_NAME})

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ${GENERATED_INCLUDE_DIR}
    ${DUMMY_TOKEN_PROVIDER_MODULE_LOADER_DIR}
    ${DUMMY_TOKEN_PROVIDER_MODULE_DIR}/main
    ${PROJECT_SOURCE_DIR}/tests/include
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
    everest::framework
    everest::staging::helpers
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
