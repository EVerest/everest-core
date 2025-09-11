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

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/asyncapi_cli_install_done
  COMMAND cd ${asyncapi-cli_DIR} && npm install && npm run build
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/generated/asyncapi_cli_install_done
  COMMENT "AsyncApi/cli Install once only"
)

add_custom_target(asyncapi_cli_install_target
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated/asyncapi_cli_install_done
)

set(ASYNCAPI_CMD ${asyncapi-cli_DIR}/bin/run)
