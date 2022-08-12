
add_library(everestjs SHARED)
target_sources(everestjs
    PRIVATE
        everestjs.cpp
        conversions.cpp
        js_exec_ctx.cpp
)

set_target_properties(everestjs PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(everestjs
    PRIVATE
        everest::framework
        everest::log
)

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
    COMMAND ${NPM} install node-addon-api
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

execute_process(
    COMMAND ${NODE} -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")

find_path(NODEJS_INCLUDE_DIR "node_api.h"
    PATH_SUFFIXES
        "node"
        "node16"
        "node17"
        "nodejs/src"
    REQUIRED
)

target_include_directories(everestjs
    PRIVATE
        $<BUILD_INTERFACE:${NODE_ADDON_API_DIR}>
        $<BUILD_INTERFACE:${NODEJS_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include
)

# define NAPI_VERSION
target_compile_definitions(everestjs
    PRIVATE
        -DNAPI_VERSION=${NODE_API_VERSION_REQUIRED}
        -DNAPI_CPP_EXCEPTIONS
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
