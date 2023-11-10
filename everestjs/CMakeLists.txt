find_program(
    NODE
    node
    REQUIRED
)

execute_process(
    COMMAND ${NODE} --version
    OUTPUT_VARIABLE NODE_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# We need Node-API version 6 or higher
set(NODE_API_VERSION_REQUIRED 6)
include(NodeApiVersion)
require_node_api_version("${NODE_VERSION}" "${NODE_API_VERSION_REQUIRED}")

find_program(
    NPM
    npm
    REQUIRED
)

# Include Node-API wrappers
# FIXME (aw): we want this as an requirement, not implicitely install it by ourself
execute_process(
    COMMAND
        ${NPM} list -p node-addon-api
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_VARIABLE
        NODE_ADDON_API_PACKAGE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Adding node-addon-api")
if (NOT NODE_ADDON_API_PACKAGE_DIR)
    execute_process(
        COMMAND
            ${NPM} install node-addon-api
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        OUTPUT_QUIET
        RESULT_VARIABLE
            NPM_INSTALL_NODE_ADDON_API_FAILED
    )

    if (NPM_INSTALL_NODE_ADDON_API_FAILED)
        message(FATAL "Installation of node-addon-api failed")
    endif ()
endif ()

execute_process(
    COMMAND ${NODE} -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")

find_path(NODEJS_INCLUDE_DIR "node_api.h"
    PATH_SUFFIXES
        "node"
        "node16"
        "node17"
        "node18"
        "node19"
        "node20"
        "nodejs/src"
    HINTS
        "$ENV{HOME}/.nvm/versions/node/${NODE_VERSION}/include"
    REQUIRED
)

add_library(everestjs SHARED)
target_sources(everestjs
    PRIVATE
        everestjs.cpp
        conversions.cpp
        js_exec_ctx.cpp
)

target_compile_options(everestjs PRIVATE ${COMPILER_WARNING_OPTIONS})

# define NAPI_VERSION
target_compile_definitions(everestjs
    PRIVATE
        -DNAPI_VERSION=${NODE_API_VERSION_REQUIRED}
        -DNAPI_CPP_EXCEPTIONS
)

set_target_properties(everestjs PROPERTIES PREFIX "" SUFFIX ".node")

target_include_directories(everestjs
    PRIVATE
        $<BUILD_INTERFACE:${NODE_ADDON_API_DIR}>
        $<BUILD_INTERFACE:${NODEJS_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(everestjs
    PRIVATE
        everest::framework
        everest::log
)

install(
    TARGETS everestjs
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/everest/node_modules/everestjs
)

install(
    FILES
        index.js
        package.json
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/everest/node_modules/everestjs
)
