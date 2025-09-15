file(GLOB_RECURSE POWER_STACK_TEST_SOURCES "*.cpp")
add_executable(user-acceptance-tests ${POWER_STACK_TEST_SOURCES})
target_include_directories(user-acceptance-tests PRIVATE include)
target_compile_options(user-acceptance-tests PRIVATE -g -O0)

target_link_libraries(user-acceptance-tests   
  fusion_charger_dispenser
  fusion_charger_goose_driver
  fusion_charger_modbus_driver
  fusion_charger_modbus_extensions
  modbus-client
  power_stack_mock_lib
  gtest_main
  gmock_main
 )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_certificates DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
