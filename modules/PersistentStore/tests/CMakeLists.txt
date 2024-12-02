set(TEST_TARGET_NAME ${PROJECT_NAME}_persistent_store_tests)

set(PERSISTENT_STORE_MODULE_DIR ${PROJECT_SOURCE_DIR}/modules/PersistentStore)

add_executable(${TEST_TARGET_NAME}
    persistent_store_tests.cpp
    ${PERSISTENT_STORE_MODULE_DIR}/main/kvsImpl.cpp
    ${PERSISTENT_STORE_MODULE_DIR}/PersistentStore.cpp
)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)
get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
set(STORE_MODULE_LOADER_DIR ${GENERATED_OUTPUT_DIR}/modules/${MODULE_NAME})

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ${GENERATED_INCLUDE_DIR}
    ${STORE_MODULE_LOADER_DIR}
    ${PERSISTENT_STORE_MODULE_DIR}/main
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gtest_main
    SQLite::SQLite3
    everest::framework
    )

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
