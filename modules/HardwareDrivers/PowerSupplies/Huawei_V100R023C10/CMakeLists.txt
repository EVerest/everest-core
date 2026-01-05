#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here

target_link_libraries(${MODULE_NAME}
  PRIVATE
  fusion_charger_dispenser
)

target_link_libraries(${MODULE_NAME}
  PRIVATE
  atomic
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "connector_1/power_supply_DCImpl.cpp"
        "connector_2/power_supply_DCImpl.cpp"
        "connector_3/power_supply_DCImpl.cpp"
        "connector_4/power_supply_DCImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
target_sources(${MODULE_NAME} PRIVATE
    "connector_base/base.cpp"
    "telemetry_publisher_everest.cpp"
)
add_subdirectory(fusion_charger_lib)

ev_register_library_target(fusion_charger_dispenser)
ev_register_library_target(goose-ethernet)
ev_register_library_target(goose)
ev_register_library_target(fusion_charger_modbus_extensions)
ev_register_library_target(fusion_charger_modbus_driver)
ev_register_library_target(fusion_charger_goose_driver)
ev_register_library_target(modbus-base)
ev_register_library_target(modbus-server)
ev_register_library_target(modbus-client)
ev_register_library_target(modbus-ssl)
ev_register_library_target(modbus-registers)

option(INSTALL_FUSION_CHARGER_MOCK "Install fusion charger mock" OFF)
if(INSTALL_FUSION_CHARGER_MOCK)
  install(TARGETS fusion_charger_mock)
endif()
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
