include(GoogleTest)

file(GLOB_RECURSE MODBUS_SERVER_TESTS_SOURCES "*.cpp")

add_executable(modbus-client-tests ${MODBUS_SERVER_TESTS_SOURCES})
target_link_libraries(modbus-client-tests PRIVATE modbus-client gtest_main)

gtest_discover_tests(modbus-client-tests)
