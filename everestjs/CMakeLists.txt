#cmake_policy(SET CMP0042 NEW)

add_library(everestjs SHARED everestjs.cpp conversions.cpp js_exec_ctx.cpp)
set_target_properties(everestjs PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(everestjs
        PRIVATE everest everest::log
)

find_program(
        NODE
        node
        REQUIRED
)

execute_process(COMMAND ${NODE} --version
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
execute_process(COMMAND ${NPM} install node-addon-api
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


execute_process(COMMAND ${NODE} -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")

find_path(NODEJS_INCLUDE_DIR "node_api.h"
        PATH_SUFFIXES
        "node"
        "node16"
        "nodejs/src"
        REQUIRED
)

target_include_directories(everestjs
    PRIVATE
        ${NODE_ADDON_API_DIR}
        ${NODEJS_INCLUDE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# needs c++ 14
target_compile_features(everestjs PRIVATE cxx_std_14)

# define NAPI_VERSION
add_definitions(-DNAPI_VERSION=${NODE_API_VERSION_REQUIRED})
add_definitions(-DNAPI_CPP_EXCEPTIONS)

install(TARGETS everestjs
        LIBRARY
        DESTINATION everestjs/node_modules/everestjs)

install(FILES index.js
        DESTINATION everestjs/node_modules/everestjs)

install(FILES package.json
        DESTINATION everestjs/node_modules/everestjs)
