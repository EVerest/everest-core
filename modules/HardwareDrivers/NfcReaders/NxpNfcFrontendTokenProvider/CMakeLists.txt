#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

set(EXTERNAL_NXPNFCRDLIB_ZIP_PATH_${MODULE_NAME} "" CACHE PATH "Set the path to NxpNfcRdLib_Linux_v07.10.00_PUB.zip to enable building the wrapper.")

target_include_directories(${MODULE_NAME}
    PRIVATE
        include/
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        everest::helpers)

# Use either the named pipe implementation as source, or the real-hardware-interfacing NxpNfcFrontend class

if(EXTERNAL_NXPNFCRDLIB_ZIP_PATH_${MODULE_NAME} AND EXISTS "${EXTERNAL_NXPNFCRDLIB_ZIP_PATH_${MODULE_NAME}}")
    set(EXTERNAL_NXPNFCRDLIB_ZIP_PATH "${EXTERNAL_NXPNFCRDLIB_ZIP_PATH_${MODULE_NAME}}")
    add_subdirectory(nxpnfcrdlib_wrapper)

    target_link_libraries(${MODULE_NAME} PRIVATE NxpNfcFrontend)
else()
    add_library(namedPipeDataSource)
    target_sources(namedPipeDataSource PRIVATE namedPipeDataSource/namedPipeDataSource.cpp)
    target_link_libraries(${MODULE_NAME} PRIVATE namedPipeDataSource)
    target_include_directories(namedPipeDataSource PUBLIC ${CMAKE_CURRENT_LIST_DIR}/namedPipeDataSource/)

    target_compile_definitions(${MODULE_NAME} PRIVATE BUILD_WITH_NAMED_PIPE_DATASOURCE)
endif()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/auth_token_providerImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
