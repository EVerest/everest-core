set(TEST_TARGET_NAME ${PROJECT_NAME}_rpcapi_tests)

set(TEST_SOURCES ${PROJECT_SOURCE_DIR}/modules/RpcApi/server/websocketServer.cpp)

add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES} server/websocketServerTests.cpp)

set(INCLUDE_DIR 
    "${PROJECT_SOURCE_DIR}/modules/RpcApi/server"
)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME}
    PUBLIC
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)

target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
        GTest::gmock
        GTest::gtest_main
        everest::timer
        everest::log
        everest::framework
        everest::staging::helpers
        websockets_shared
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
