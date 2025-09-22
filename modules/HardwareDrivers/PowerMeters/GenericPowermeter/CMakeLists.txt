#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/models/
    DESTINATION "${CMAKE_INSTALL_DATADIR}/everest/modules/${MODULE_NAME}/models/"
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/powermeterImpl.cpp"
)

target_compile_features(${MODULE_NAME} PRIVATE cxx_std_17)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
target_link_libraries(${MODULE_NAME} PRIVATE everest::framework)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
