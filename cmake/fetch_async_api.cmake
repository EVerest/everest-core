if(EVEREST_BUILD_DOCS)
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
endif()

if(EVEREST_BUILD_DOCS)
  find_package(asyncapi-html-template
    COMPONENTS bundling
    PATHS ../asyncapi-html-template
  )

  if(NOT asyncapi-html-template_FOUND)
    message(STATUS "Retrieving asyncapi-html-template using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
      asyncapi-html-template
      GIT_REPOSITORY https://github.com/asyncapi/html-template.git
      GIT_TAG v3.0.0
    )
    FetchContent_MakeAvailable(asyncapi-html-template)
    set(asyncapi-html-template_DIR "${asyncapi-html-template_SOURCE_DIR}")
    set(asyncapi-html-template_FIND_COMPONENTS "bundling")
  endif()

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/asyncapi_html_template_install_done
    COMMAND cd ${asyncapi-html-template_SOURCE_DIR} && npm install
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/generated/asyncapi_html_template_install_done
    COMMENT "AsyncApi/html-template Install once only"
  )

  add_custom_target(asyncapi_html_template_install_target
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated/asyncapi_html_template_install_done
  )
endif()
