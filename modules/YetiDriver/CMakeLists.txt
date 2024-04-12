#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
add_subdirectory(yeti_comms)
add_subdirectory(yeti_fwupdate)

target_include_directories(${MODULE_NAME}
    PRIVATE
        "common"
        "yeti_comms"
        "yeti_comms/nanopb"
        "yeti_comms/protobuf"
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        Pal::Sigslot
        yeti_comms
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "powermeter/powermeterImpl.cpp"
        "board_support/evse_board_supportImpl.cpp"
        "rcd/ac_rcdImpl.cpp"
        "connector_lock/connector_lockImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
install(FILES yetiR1_2.1_firmware.bin DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/YetiDriver/firmware)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
