#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
add_subdirectory(tida_010939_comms)

target_include_directories(${MODULE_NAME}
    PRIVATE
        "common"
        "tida_010939_comms"
        "tida_010939_comms/nanopb"
        "tida_010939_comms/protobuf"
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        Pal::Sigslot
        tida_010939_comms
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "board_support/evse_board_supportImpl.cpp"
        "rcd/ac_rcdImpl.cpp"
        "connector_lock/connector_lockImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
# install(FILES firmware.bin DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/TIDA010939BSP/firmware)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
