#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
add_subdirectory(evyeti_comms)

target_include_directories(${MODULE_NAME}
    PRIVATE
        "common"
        "evyeti_comms"
        "evyeti_comms/nanopb"
        "evyeti_comms/protobuf"
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        Pal::Sigslot
        evyeti_comms
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "ev_board_support/ev_board_supportImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
