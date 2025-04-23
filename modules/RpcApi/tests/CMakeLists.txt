set(TEST_TARGET_NAME ${PROJECT_NAME}_rpcapi_tests)

set(TEST_SOURCES 
    ${PROJECT_SOURCE_DIR}/modules/RpcApi/rpc/RpcHandler.cpp
    ${PROJECT_SOURCE_DIR}/modules/RpcApi/server/WebsocketServer.cpp
    server/WebsocketServerTests.cpp
    rpc/RpcHandlerTests.cpp
)

add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES})

# The following is needed to import target compile definitions from the module
get_target_property(RPCAPI_COMPILE_DEFINITIONS ${MODULE_NAME} COMPILE_DEFINITIONS)

if (RPCAPI_COMPILE_DEFINITIONS)
    target_compile_definitions(${TEST_TARGET_NAME} PRIVATE ${RPCAPI_COMPILE_DEFINITIONS})
endif()

set(INCLUDE_DIR 
    "${PROJECT_SOURCE_DIR}/modules/RpcApi/server"
    "${PROJECT_SOURCE_DIR}/modules/RpcApi/rpc"
    "${PROJECT_SOURCE_DIR}/modules/RpcApi/data"
)

target_sources(${TEST_TARGET_NAME}
    PRIVATE
        "helpers/WebSocketTestClient.cpp"
)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME}
    PUBLIC
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)

target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
        GTest::gtest_main
        everest::log
        websockets_shared
        json-rpc-cxx
        nlohmann_json::nlohmann_json
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
