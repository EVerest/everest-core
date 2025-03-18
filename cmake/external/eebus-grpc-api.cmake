include_guard(GLOBAL)

if (NOT CPM_PACKAGE_eebus-grpc-api_SOURCE_DIR)
    message(FATAL_ERROR "CPM_PACKAGE_eebus-grpc-api_SOURCE_DIR not set")
endif()

set(EEBUS_GRPC_API_SOURCE_DIR ${CPM_PACKAGE_eebus-grpc-api_SOURCE_DIR})
set(EEBUS_GRPC_API_BINARY_DIR ${CMAKE_BINARY_DIR}/eebus-grpc-api)

add_go_target(
    NAME
        eebus_grpc_api_cmd
    OUTPUT
        ${EEBUS_GRPC_API_BINARY_DIR}/cmd
    GO_PACKAGE_SOURCE_PATH
        ${EEBUS_GRPC_API_SOURCE_DIR}/cmd
    OUTPUT_DIRECTORY
        ${EEBUS_GRPC_API_BINARY_DIR}
    WORKING_DIRECTORY
        ${EEBUS_GRPC_API_SOURCE_DIR}
)

add_go_target(
    NAME
        eebus_grpc_api_create_cert
    OUTPUT
        ${EEBUS_GRPC_API_BINARY_DIR}/create_cert
    GO_PACKAGE_SOURCE_PATH
        ${EEBUS_GRPC_API_SOURCE_DIR}/create_cert
    OUTPUT_DIRECTORY
        ${EEBUS_GRPC_API_BINARY_DIR}
    WORKING_DIRECTORY
        ${EEBUS_GRPC_API_SOURCE_DIR}
)

add_go_target(
    NAME
        eebus_grpc_api_dummy_cem
    OUTPUT
        ${EEBUS_GRPC_API_BINARY_DIR}/dummy_cem
    GO_PACKAGE_SOURCE_PATH
        ${EEBUS_GRPC_API_SOURCE_DIR}/dummy_cem
    OUTPUT_DIRECTORY
        ${EEBUS_GRPC_API_BINARY_DIR}
    WORKING_DIRECTORY
        ${EEBUS_GRPC_API_SOURCE_DIR}
)

add_custom_target(eebus_grpc_api_all
    DEPENDS
        eebus_grpc_api_cmd
        eebus_grpc_api_create_cert
        eebus_grpc_api_dummy_cem
)
