#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
add_subdirectory(phyverso_mcu_comms)
add_subdirectory(phyverso_cli)
add_subdirectory(phyverso_gpio)

target_include_directories(${MODULE_NAME}
    PRIVATE
        "common"
        "phyverso_mcu_comms"
        "phyverso_mcu_comms/nanopb"
        "phyverso_mcu_comms/protobuf"
        "phyverso_config"
        "phyverso_gpio"
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        Pal::Sigslot
        everest::gpio
        phyverso_mcu_comms
        phyverso_config
        phyverso_gpio
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "connector_1/evse_board_supportImpl.cpp"
        "connector_2/evse_board_supportImpl.cpp"
        "rcd_1/ac_rcdImpl.cpp"
        "rcd_2/ac_rcdImpl.cpp"
        "connector_lock_1/connector_lockImpl.cpp"
        "connector_lock_2/connector_lockImpl.cpp"
        "phyverso_mcu_temperature/phyverso_mcu_temperatureImpl.cpp"
        "system_specific_data_1/generic_arrayImpl.cpp"
        "system_specific_data_2/generic_arrayImpl.cpp"
        "led_driver_1/led_driverImpl.cpp"
        "led_driver_2/led_driverImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
target_sources(${MODULE_NAME}
    PRIVATE
        "board_support_common.cpp"
)

# install MCU configs
# List all JSON files in the current directory and subdirectories
file(GLOB_RECURSE JSON_FILES "*.json")

# Check if the list is not empty
if(JSON_FILES)
    # Install JSON files
    install(FILES ${JSON_FILES} DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/everest")
endif()
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
