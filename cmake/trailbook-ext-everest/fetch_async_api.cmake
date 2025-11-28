if(asyncapi-cli_DIR)
    message(STATUS "Using asyncapi-cli at this location: ${asyncapi-cli_DIR}")
else()
    message(STATUS "Retrieving asyncapi-cli using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
      asyncapi-cli
      GIT_REPOSITORY https://github.com/asyncapi/cli.git
      GIT_TAG v2.7.1
    )
    FetchContent_MakeAvailable(asyncapi-cli)
    set(asyncapi-cli_DIR "${asyncapi-cli_SOURCE_DIR}")
    set(asyncapi-cli_FIND_COMPONENTS "bundling")
endif()

set(ASYNCAPI_CLI_INSTALL_SENTINEL_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(ASYNCAPI_CLI_INSTALL_SENTINEL "${ASYNCAPI_CLI_INSTALL_SENTINEL_PATH}/asyncapi_cli_install_done")

if(NOT TARGET asyncapi_cli_install_target)
    add_custom_command(
        OUTPUT ${ASYNCAPI_CLI_INSTALL_SENTINEL}

        # Do installation
        COMMAND ${CMAKE_COMMAND} -E chdir ${asyncapi-cli_DIR} npm install
        COMMAND ${CMAKE_COMMAND} -E chdir ${asyncapi-cli_DIR} npm run build

        # Create sentinel file
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ASYNCAPI_CLI_INSTALL_SENTINEL_PATH}
        COMMAND ${CMAKE_COMMAND} -E touch ${ASYNCAPI_CLI_INSTALL_SENTINEL}

        COMMENT "AsyncApi/cli Install once only"
    )

    add_custom_target(asyncapi_cli_install_target
        DEPENDS ${ASYNCAPI_CLI_INSTALL_SENTINEL}
    )
else()
    message(STATUS "Skipping definition of 'asyncapi_cli_install_target'; already exists.")
endif()

set(ASYNCAPI_CMD ${asyncapi-cli_DIR}/bin/run)