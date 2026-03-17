set(TEST_TARGET_NAME ${PROJECT_NAME}_energy_node_tests)
# Compile the test file and the utility implementation
add_executable(${TEST_TARGET_NAME})

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

set(INCLUDE_DIR
    "${MODULE_DIR}/include"
    "${MODULE_DIR}/tests"
    ..
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ${INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
)

target_sources(${TEST_TARGET_NAME} PRIVATE
    energy_node_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../energy_grid/energy_schedule_utils.cpp
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
    everest::log
    everest::framework
    everest::util
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
