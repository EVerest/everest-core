#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
include(${CMAKE_SOURCE_DIR}/cmake/external/eebus-grpc-api.cmake)
include(cmake/setup-generator-tooling.cmake)
include(cmake/generate.cmake)

get_target_property(EEBUS_GRPC_API_BINARY_PATH eebus_grpc_api_cmd TARGET_FILE)
if (EEBUS_GRPC_API_BINARY_PATH EQUAL "EEBUS_GRPC_API_BINARY_PATH-NOTFOUND")
    message(FATAL_ERROR "Couldn't get target property TARGET_FILE from eebus_grpc_api_cmd")
endif()
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/compile_time_settings.hpp.in"
    ${CMAKE_CURRENT_BINARY_DIR}/generated/compile_time_settings.hpp
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/emptyImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
target_sources(${MODULE_NAME}
    PRIVATE
        "helper.cpp"
        "UseCaseEventReader.cpp"
        "grpc_calls/ControllableSystemLPCControlCalls.cpp"
        "grpc_calls/ControlServiceCalls.cpp"
        "ConfigValidator.cpp"
)
target_link_libraries(${MODULE_NAME}
    PUBLIC
        proto_control_service
        proto_cs_lpc_service
        everest::staging::run_application
)
add_dependencies(${MODULE_NAME} eebus_grpc_api_cmd)
target_include_directories(${MODULE_NAME}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/generated
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
