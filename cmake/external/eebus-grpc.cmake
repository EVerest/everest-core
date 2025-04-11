include_guard(GLOBAL)

if (NOT CPM_PACKAGE_eebus-grpc_SOURCE_DIR)
    message(FATAL_ERROR "CPM_PACKAGE_eebus-grpc_SOURCE_DIR not set")
endif()

set(EEBUS_GRPC_API_SOURCE_DIR ${CPM_PACKAGE_eebus-grpc_SOURCE_DIR})
set(EEBUS_GRPC_API_BINARY_DIR ${CMAKE_BINARY_DIR}/eebus-grpc)

file(GLOB_RECURSE EEBUS_GRPC_API_SOURCE_FILES
    CONFIGURE_DEPENDS
    ${EEBUS_GRPC_API_SOURCE_DIR}/*
)

setup_go()
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
    DEPENDS
        ${EEBUS_GRPC_API_SOURCE_FILES}
)

add_go_target(
    NAME
        eebus_grpc_api_create_cert
    OUTPUT
        ${EEBUS_GRPC_API_BINARY_DIR}/create_cert
    GO_PACKAGE_SOURCE_PATH
        ${EEBUS_GRPC_API_SOURCE_DIR}/cmd/create_cert
    OUTPUT_DIRECTORY
        ${EEBUS_GRPC_API_BINARY_DIR}
    WORKING_DIRECTORY
        ${EEBUS_GRPC_API_SOURCE_DIR}
    DEPENDS
        ${EEBUS_GRPC_API_SOURCE_FILES}
)

add_go_target(
    NAME
        eebus_grpc_api_dummy_cem
    OUTPUT
        ${EEBUS_GRPC_API_BINARY_DIR}/dummy_cem
    GO_PACKAGE_SOURCE_PATH
        ${EEBUS_GRPC_API_SOURCE_DIR}/cmd/dummy_cem
    OUTPUT_DIRECTORY
        ${EEBUS_GRPC_API_BINARY_DIR}
    WORKING_DIRECTORY
        ${EEBUS_GRPC_API_SOURCE_DIR}
    DEPENDS
        ${EEBUS_GRPC_API_SOURCE_FILES}
)

add_custom_target(eebus_grpc_api_all
    DEPENDS
        eebus_grpc_api_cmd
        eebus_grpc_api_create_cert
        eebus_grpc_api_dummy_cem
)

install(
    FILES
        ${EEBUS_GRPC_API_BINARY_DIR}/cmd
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/bin
    RENAME
        eebus_grpc_api
    PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_READ
        WORLD_EXECUTE WORLD_READ
)

# This function creates certificates for an EEBus component
#
# It will generate thre files:
# - <OUT_DIR>/<NAME>_cert
# - <OUT_DIR>/<NAME>_key
# - <OUT_DIR>/<NAME>_ski
# The generation can be triggered by calling the target <TARGET_NAME>
#
function(eebus_create_cert)
    set(options)
    set(one_value_args
        NAME
        OUT_DIR
        TARGET_NAME
        OUT_FILES_VAR
    )
    set(multi_value_args)
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if (arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unparsed arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()
    if (arg_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "Keywords missing values: ${arg_KEYWORDS_MISSING_VALUES}")
    endif()
    if (NOT arg_NAME)
        message(FATAL_ERROR "NAME not set")
    endif()
    if (NOT arg_OUT_DIR)
        message(FATAL_ERROR "OUT_DIR not set")
    endif()
    if (NOT IS_ABSOLUTE ${arg_OUT_DIR})
        message(FATAL_ERROR "OUT_DIR ${arg_OUT_DIR} is not an absolute path")
    endif()
    if (NOT arg_TARGET_NAME)
        message(FATAL_ERROR "TARGET_NAME not set")
    endif()

    get_target_property(CREATE_CERT_BINARY_FILE eebus_grpc_api_create_cert TARGET_FILE)
    set(OUT_FILES
        ${arg_OUT_DIR}/${arg_NAME}_cert
        ${arg_OUT_DIR}/${arg_NAME}_key
        ${arg_OUT_DIR}/${arg_NAME}_ski
    )
    add_custom_command(
        OUTPUT
            ${OUT_FILES}
        COMMAND
            mkdir -p ${arg_OUT_DIR}
        COMMAND
            ${CREATE_CERT_BINARY_FILE}
        ARGS
            ${arg_OUT_DIR}
            ${arg_NAME}
        DEPENDS
            eebus_grpc_api_create_cert
        COMMENT
            "Creating ${arg_NAME} certificates"
    )
    add_custom_target(${arg_TARGET_NAME}
        DEPENDS
            ${OUT_FILES}
    )

    if (arg_OUT_FILES_VAR)
        set(${arg_OUT_FILES_VAR} ${OUT_FILES} PARENT_SCOPE)
    endif()
endfunction()
