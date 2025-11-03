file(GLOB SOURCES "src/*.cpp")

add_library(fusion_charger_modbus_extensions STATIC ${SOURCES})
target_include_directories(fusion_charger_modbus_extensions PUBLIC include)
target_link_libraries(fusion_charger_modbus_extensions PUBLIC modbus-server modbus-registers)

if(EVEREST_CORE_BUILD_TESTING)
  include(GoogleTest)

  file(GLOB TEST_SOURCES "tests/*.cpp")
  add_executable(fusion_charger_modbus_extensions_test ${TEST_SOURCES})
  target_link_libraries(fusion_charger_modbus_extensions_test PRIVATE fusion_charger_modbus_extensions gtest_main)
  gtest_discover_tests(fusion_charger_modbus_extensions_test)
endif()
