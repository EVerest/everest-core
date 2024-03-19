#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
#
# this does not create any archive and is just an internal test helper.
add_library(sunspec_framework_object_lib
    OBJECT
    lib/protocol_related_types.cpp
    lib/transport.cpp
    lib/known_model.cpp
    )

target_include_directories( sunspec_framework_object_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
target_include_directories( sunspec_framework_object_lib PUBLIC ${CMAKE_BINARY_DIR}/generated/include )
add_dependencies( sunspec_framework_object_lib generate_cpp_files )

target_link_libraries( sunspec_framework_object_lib
    PUBLIC
    everest::modbus
    everest::framework
    )

if (BUILD_DEV_TESTS)
    add_subdirectory( tests )
endif()

#
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/powermeterImpl.cpp"
        "ac_meter/sunspec_ac_meterImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
target_link_libraries( ${MODULE_NAME}
    PRIVATE
    sunspec_framework_object_lib )
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
