file(GLOB SOURCES "src/*.cpp")

add_library(fusion_charger_goose_driver STATIC ${SOURCES})
target_include_directories(fusion_charger_goose_driver PUBLIC include)
target_link_libraries(fusion_charger_goose_driver PUBLIC goose)

if(EVEREST_CORE_BUILD_TESTING)
  include(GoogleTest)

  file(GLOB TEST_SOURCES "tests/*.cpp")
  add_executable(fusion_charger_goose_driver_test ${TEST_SOURCES})
  target_link_libraries(fusion_charger_goose_driver_test PRIVATE fusion_charger_goose_driver gtest_main)
  gtest_discover_tests(fusion_charger_goose_driver_test)
endif()
