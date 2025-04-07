#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here

target_compile_features(${MODULE_NAME} PRIVATE cxx_std_17)
set_target_properties(${MODULE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

if (DISABLE_EDM)
    find_package(libwebsockets REQUIRED)
    find_package(json-rpc-cxx REQUIRED)
endif()

target_link_libraries(${MODULE_NAME}
    PRIVATE
        websockets_shared
        json-rpc-cxx
)

target_sources(${MODULE_NAME}
    PRIVATE
        "data/Data.cpp"
        "server/TransportInterface.cpp"
        "server/WebsocketServer.cpp"
        "rpc/RpcHandler.cpp"
)

target_compile_options(${MODULE_NAME}
    PRIVATE
        -Wimplicit-fallthrough
#        -Werror=switch-enum
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/emptyImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
