file(GLOB SOURCES "src/*.cpp")

add_library(modbus-registers STATIC ${SOURCES})
target_include_directories(modbus-registers PUBLIC include)
target_link_libraries(modbus-registers PUBLIC modbus-server)

if(EVEREST_CORE_BUILD_TESTING)
  # Create test executable
  include(GoogleTest)

  file(GLOB TEST_SOURCES "tests/*.cpp")
  add_executable(modbus-registers-tests ${TEST_SOURCES})
  target_link_libraries(modbus-registers-tests modbus-registers GTest::gtest_main)

  gtest_discover_tests(modbus-registers-tests)
endif()
