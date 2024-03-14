set(TEST_TARGET_NAME ${PROJECT_NAME}_setup_tests)
add_executable(${TEST_TARGET_NAME})

target_include_directories(${TEST_TARGET_NAME} PUBLIC ${GTEST_INCLUDE_DIRS} . ..)

target_sources(${TEST_TARGET_NAME} PRIVATE
    RunApplicationStub.cpp
    WiFiSetupTest.cpp
    ../WiFiSetup.cpp
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gtest_main
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
