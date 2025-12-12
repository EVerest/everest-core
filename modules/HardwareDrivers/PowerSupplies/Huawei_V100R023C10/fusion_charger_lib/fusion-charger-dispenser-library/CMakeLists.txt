if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

file(GLOB LIB_SOURCES lib/*.cpp)
add_library(fusion_charger_dispenser STATIC ${LIB_SOURCES})
ev_register_library_target(fusion_charger_dispenser)
target_include_directories(fusion_charger_dispenser PUBLIC include)
target_link_libraries(fusion_charger_dispenser PUBLIC fusion_charger_goose_driver fusion_charger_modbus_driver fusion_charger_modbus_extensions modbus-ssl Huawei::FusionCharger::LogInterface)

option(BUILD_ACCEPTANCE_TESTS "Build acceptance tests" OFF)
if(EVEREST_CORE_BUILD_TESTING)
  add_subdirectory(tests)

  if (BUILD_ACCEPTANCE_TESTS)
    add_subdirectory(user-acceptance-tests)
  endif()

endif()

add_subdirectory(power_stack_mock)
