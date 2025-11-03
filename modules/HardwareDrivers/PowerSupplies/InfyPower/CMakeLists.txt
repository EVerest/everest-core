#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
# add_subdirectory(test-tool)
target_sources(${MODULE_NAME}
    PRIVATE
    "can_driver_acdc/CanBus.cpp"
    "can_driver_acdc/CanPackets.cpp"
    "can_driver_acdc/InfyCanDevice.cpp"
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        Pal::Sigslot
        everest::io
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/power_supply_DCImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
