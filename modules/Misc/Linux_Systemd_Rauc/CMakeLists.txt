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
        Pal::Sigslot
        everest::run_application
        everest::system
)

target_sources(${MODULE_NAME}
    PRIVATE
        "diagnostics_handler.cpp"
        "rauc_dbus.cpp"
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/systemImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
install(
    PROGRAMS
        constants.env
        diagnostics_uploader.sh
        diagnostics_collector.sh
    DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
