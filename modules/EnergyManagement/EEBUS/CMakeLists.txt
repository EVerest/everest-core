#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
include(${CMAKE_SOURCE_DIR}/cmake/external/eebus-grpc.cmake)

add_subdirectory(grpc_libs)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/emptyImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
target_sources(${MODULE_NAME}
    PRIVATE
        "helper.cpp"
        "ConfigValidator.cpp"
        "EebusCallbacks.cpp"
        "EebusConnectionHandler.cpp"
        "LpcUseCaseHandler.cpp"
        "UseCaseEventReader.cpp"
)
target_link_libraries(${MODULE_NAME}
    PUBLIC
        proto_control_service
        proto_cs_lpc_service
        everest::run_application
        everest::io
)
add_dependencies(${MODULE_NAME} eebus_grpc_api_cmd)
target_include_directories(${MODULE_NAME}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/generated
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

get_target_property(EEBUS_GRPC_API_CMD_BINARY_PATH eebus_grpc_api_cmd TARGET_FILE)
install(
    FILES
        ${EEBUS_GRPC_API_CMD_BINARY_PATH}
    DESTINATION
        "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
    RENAME
        eebus_grpc_api
    PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_READ
        WORLD_EXECUTE WORLD_READ
)

if (EEBUS_GENERATE_AND_INSTALL_CERTIFICATES)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/eebus-generate-and-install-certificates.cmake)
endif()
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
