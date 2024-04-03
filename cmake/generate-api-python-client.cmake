#
# Licensor: Pionix GmbH, 2024
# License: BaseCamp - License Version 1.0
#
# Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
# under: https://pionix.com/pionix-license-terms
# You may not use this file/code except in compliance with said License.
#

function(generate_async_python_client)
    set(oneValueArgs
        API_PATH
        NAME_CAMEL_CASE
        NAME_SNAKE_CASE
        INTERFACE_PREFIX
    )
    cmake_parse_arguments(FUNCTION_ARGS "" "${oneValueArgs}" "" ${ARGN})
    if ("${FUNCTION_ARGS_API_PATH}" STREQUAL "")
        message(FATAL_ERROR "API_PATH is required")
    endif()
    if ("${FUNCTION_ARGS_NAME_CAMEL_CASE}" STREQUAL "")
        message(FATAL_ERROR "NAME_CAMEL_CASE is required")
    endif()
    if ("${FUNCTION_ARGS_NAME_SNAKE_CASE}" STREQUAL "")
        message(FATAL_ERROR "NAME_SNAKE_CASE is required")
    endif()
    if ("${FUNCTION_ARGS_INTERFACE_PREFIX}" STREQUAL "")
        message(FATAL_ERROR "INTERFACE_PREFIX is required")
    endif()

    set(CHECK_DONE_FILE ${CMAKE_CURRENT_BINARY_DIR}/generated/async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE}_done)
    set(TEMPLATE_DIR ${python-mqtt-template_SOURCE_DIR})
    if ("${TEMPLATE_DIR}" STREQUAL "")
        message(FATAL_ERROR "Could not find python-mqtt-template, please install it using CPM or set python-mqtt-template_SOURCE_DIR")
    endif()
    set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE})
    add_custom_target(async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE}
        DEPENDS
            ${CHECK_DONE_FILE}
            asyncapi_cli_install_target
            npm_install_target
    )
    add_custom_command(
        OUTPUT      ${CHECK_DONE_FILE}
        COMMAND     flock -x ${TEMPLATE_DIR} ${ASYNCAPI_CMD} generate fromTemplate ${FUNCTION_ARGS_API_PATH} ${TEMPLATE_DIR} -o ${OUTPUT_DIR} -p server=default -p nameCamelCase=${FUNCTION_ARGS_NAME_CAMEL_CASE} -p nameSnakeCase=${FUNCTION_ARGS_NAME_SNAKE_CASE} -p interfacePrefix=${FUNCTION_ARGS_INTERFACE_PREFIX} --force-write
        COMMAND     ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE}
        DEPENDS     ${FUNCTION_ARGS_API_PATH} ${TEMPLATE_DIR}
        COMMENT     "Generating async python client for ${FUNCTION_ARGS_NAMEL_CAMEL_CASE} from ${FUNCTION_ARGS_API_PATH}"
    )
    message(STATUS "Added target async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE} to generate async python client for ${FUNCTION_ARGS_NAME_CAMEL_CASE} from ${FUNCTION_ARGS_API_PATH}")

    ev_create_python_wheel_targets(
        PACKAGE_NAME                async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE}
        PACKAGE_SOURCE_DIRECTORY    ${OUTPUT_DIR}
        DEPENDS                     async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE}
    )

    ev_create_pip_install_targets(
        PACKAGE_NAME                async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE}
        PACKAGE_SOURCE_DIRECTORY    ${OUTPUT_DIR}
        DEPENDS                     async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE}
    )
endfunction()

function(add_api_client)
    set(oneValueArgs
        API_PATH
        NAME_CAMEL_CASE
        NAME_SNAKE_CASE
        INTERFACE_PREFIX
    )
    cmake_parse_arguments(FUNCTION_ARGS
        ""
        "${oneValueArgs}"
        ""
        ${ARGN}
    )
    if ("${FUNCTION_ARGS_API_PATH}" STREQUAL "")
        message(FATAL_ERROR "API_PATH is required")
    endif()
    if ("${FUNCTION_ARGS_NAME_CAMEL_CASE}" STREQUAL "")
        message(FATAL_ERROR "NAME_CAMEL_CASE is required")
    endif()
    if ("${FUNCTION_ARGS_NAME_SNAKE_CASE}" STREQUAL "")
        message(FATAL_ERROR "NAME_SNAKE_CASE is required")
    endif()
    if ("${FUNCTION_ARGS_INTERFACE_PREFIX}" STREQUAL "")
        message(FATAL_ERROR "INTERFACE_PREFIX is required")
    endif()
    generate_async_python_client(
        API_PATH            ${FUNCTION_ARGS_API_PATH}
        NAME_CAMEL_CASE     ${FUNCTION_ARGS_NAME_CAMEL_CASE}
        NAME_SNAKE_CASE     ${FUNCTION_ARGS_NAME_SNAKE_CASE}
        INTERFACE_PREFIX    ${FUNCTION_ARGS_INTERFACE_PREFIX}
    )
    add_dependencies(basecamp_api_clients
        async_python_client_${FUNCTION_ARGS_NAME_SNAKE_CASE}_install_wheel
   )
endfunction()

