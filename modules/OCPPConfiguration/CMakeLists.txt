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
        ryml::ryml
)

target_sources(${MODULE_NAME}
        PRIVATE
        "example_module/example_userImpl.cpp"
        "main/config_writer.cpp"
        "main/mapping_reader.cpp"
        "main/util.cpp"
        "main/event_handler.cpp"
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
target_compile_features(${MODULE_NAME} PRIVATE cxx_std_17)
set_target_properties(${MODULE_NAME} PROPERTIES CXX_EXTENSIONS OFF)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
