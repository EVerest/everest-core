#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        main/slac_io.cpp
        main/context.cpp
        main/states/others.cpp
        main/states/matching.cpp
        main/states/matching_handle_slac.cpp
        main/fsm_controller.cpp
)

target_include_directories(${MODULE_NAME}
    PRIVATE
        libfsm
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        slac::slac
        fsm::fsm
        Pal::Sigslot
)

add_executable(slac_test)
target_sources(slac_test
    PRIVATE
        main/slac_test.cpp
        main/context.cpp
        main/states/others.cpp
        main/states/matching.cpp
        main/states/matching_handle_slac.cpp
)

target_include_directories(slac_test
    PRIVATE
        libfsm
)

target_link_libraries(slac_test
    PRIVATE
        slac::slac
        fmt::fmt
)
target_compile_features(slac_test PRIVATE cxx_std_17)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/slacImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
