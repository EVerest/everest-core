include(GoogleTest)

file(GLOB_RECURSE MODBUS_SERVER_TESTS_SOURCES "*.cpp")

add_executable(modbus-base-tests ${MODBUS_SERVER_TESTS_SOURCES})
target_link_libraries(modbus-base-tests PRIVATE modbus-base gtest_main)

gtest_discover_tests(modbus-base-tests)
