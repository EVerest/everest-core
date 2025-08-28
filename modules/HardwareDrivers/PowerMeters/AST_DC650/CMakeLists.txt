#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
target_sources(${MODULE_NAME}
    PRIVATE
    crc16.cpp
    slip_protocol.cpp
    serial_device.cpp
    ast_app_layer.cpp
    diagnostics.cpp
)

target_compile_features(${MODULE_NAME} PUBLIC cxx_std_17)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/powermeterImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
target_link_libraries(${MODULE_NAME} PRIVATE everest::framework)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
