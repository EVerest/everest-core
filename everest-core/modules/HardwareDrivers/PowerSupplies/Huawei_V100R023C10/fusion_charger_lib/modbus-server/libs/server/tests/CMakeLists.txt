include(GoogleTest)

file(GLOB_RECURSE MODBUS_SERVER_TESTS_SOURCES "*.cpp")

add_executable(modbus-server-tests ${MODBUS_SERVER_TESTS_SOURCES})
target_link_libraries(modbus-server-tests PRIVATE modbus-server gtest_main)

gtest_discover_tests(modbus-server-tests)
