include(GoogleTest)

file(GLOB TEST_SOURCES "*.cpp")
add_executable(modbus-registers-tests ${TEST_SOURCES})
target_link_libraries(modbus-registers-tests PRIVATE modbus-registers GTest::gtest_main)
gtest_discover_tests(modbus-registers-tests)
