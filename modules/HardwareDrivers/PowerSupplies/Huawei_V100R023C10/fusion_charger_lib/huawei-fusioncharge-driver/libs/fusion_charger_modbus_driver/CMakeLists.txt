file(GLOB SOURCES "src/*.cpp")

add_library(fusion_charger_modbus_driver STATIC ${SOURCES})
ev_register_library_target(fusion_charger_modbus_driver)
target_include_directories(fusion_charger_modbus_driver PUBLIC include)
target_link_libraries(fusion_charger_modbus_driver PUBLIC modbus-server fusion_charger_modbus_extensions)

if(BUILD_TESTING)
  include(GoogleTest)

  file(GLOB TEST_SOURCES "tests/*.cpp")
  add_executable(fusion_charger_modbus_driver_test ${TEST_SOURCES})
  target_link_libraries(fusion_charger_modbus_driver_test PRIVATE fusion_charger_modbus_driver gtest_main)
  gtest_discover_tests(fusion_charger_modbus_driver_test)
endif()
