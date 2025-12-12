include(GoogleTest)

file(GLOB SOURCES "src/*.cpp")

add_library(modbus-registers STATIC ${SOURCES})
ev_register_library_target(modbus-registers)
target_include_directories(modbus-registers PUBLIC include)
target_link_libraries(modbus-registers PUBLIC modbus-server)

if(EVEREST_CORE_BUILD_TESTING)
  file(GLOB TEST_SOURCES "tests/*.cpp")
  add_executable(modbus-registers-tests ${TEST_SOURCES})
  target_link_libraries(modbus-registers-tests PRIVATE modbus-registers GTest::gtest_main)
  gtest_discover_tests(modbus-registers-tests)
endif()
