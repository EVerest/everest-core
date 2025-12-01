if(asyncapi-html-template_DIR)
    message(STATUS "Using existing asyncapi-html-template location: ${asyncapi-html-template_DIR}")
else()
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
endif()

set(ASYNCAPI_HTML_TEMPLATE_INSTALL_SENTINEL_PATH "${CMAKE_CURRENT_BINARY_DIR}/generate")
set(ASYNCAPI_HTML_TEMPLATE_INSTALL_SENTINEL "${ASYNCAPI_HTML_TEMPLATE_INSTALL_SENTINEL_PATH}/asyncapi_html_template_install_done")

if(NOT TARGET asyncapi_html_template_install_target)
    add_custom_command(
        OUTPUT ${ASYNCAPI_HTML_TEMPLATE_INSTALL_SENTINEL}

        # Do installation
        COMMAND ${CMAKE_COMMAND} -E chdir ${asyncapi-html-template_DIR} npm install

        # Create sentinel file
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ASYNCAPI_HTML_TEMPLATE_INSTALL_SENTINEL_PATH}
        COMMAND ${CMAKE_COMMAND} -E touch ${ASYNCAPI_HTML_TEMPLATE_INSTALL_SENTINEL}

        COMMENT "AsyncApi/html-template Install once only"
    )

    add_custom_target(asyncapi_html_template_install_target
        DEPENDS ${ASYNCAPI_HTML_TEMPLATE_INSTALL_SENTINEL}
    )
else()
    message(STATUS "Skipping definition of 'asyncapi_html_template_install_target'; already exists.")
endif()