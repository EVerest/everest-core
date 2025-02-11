#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
target_compile_options(${MODULE_NAME}
    PRIVATE
        -Wimplicit-fallthrough
        -Werror=switch-enum
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "evse/slacImpl.cpp"
        "ev/ev_slacImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
target_sources(${MODULE_NAME}
        PRIVATE
        "util/state.cpp"
)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
