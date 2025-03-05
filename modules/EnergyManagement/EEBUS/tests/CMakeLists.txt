set(TEST_TARGET_NAME ${PROJECT_NAME}_EEBUS_lpc_state_machine_tests)
add_executable(${TEST_TARGET_NAME})

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME}
    PRIVATE
        ..
        ../include
        ${GENERATED_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/generated/modules/EEBUS
)

target_sources(${TEST_TARGET_NAME}
    PRIVATE
        lpc_state_machine_tests.cpp
        ../LpcUseCaseHandler.cpp
        ../helper.cpp
        ../EebusCallbacks.cpp
)

target_compile_definitions(${TEST_TARGET_NAME}
    PRIVATE
        BUILD_TESTING_MODULE_EEBUS
)

target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
        GTest::gtest_main
        GTest::gmock
        everest::log
        everest::framework
        proto_control_service
        proto_cs_lpc_service
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
