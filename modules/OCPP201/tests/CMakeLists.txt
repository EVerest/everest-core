set(TEST_TARGET_NAME ${PROJECT_NAME}transaction_handler_tests)
add_executable(${TEST_TARGET_NAME} transaction_handler_tests.cpp)

set(INCLUDE_DIR 
    "${PROJECT_SOURCE_DIR}/modules/OCPP201")

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ${INCLUDE_DIR}
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    everest::ocpp
    GTest::gtest_main
    )

target_sources(${TEST_TARGET_NAME} PRIVATE "../transaction_handler.cpp")

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
