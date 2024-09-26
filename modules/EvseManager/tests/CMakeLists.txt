set(TEST_TARGET_NAME ${PROJECT_NAME}_EvseManager_tests)
add_executable(${TEST_TARGET_NAME})

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
    .. ../../../tests/include
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
)

target_sources(${TEST_TARGET_NAME} PRIVATE
    EventQueueTest.cpp
    IECStateMachineTest.cpp
    ../IECStateMachine.cpp
    ../backtrace.cpp
)

target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
    BUILD_TESTING_MODULE_EVSE_MANAGER
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
    everest::log
    everest::framework
    sigslot
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

set(UTIL_TEST_TARGET ${MODULE_NAME}_util)
add_executable(${UTIL_TEST_TARGET} util.cpp)

target_compile_definitions(${UTIL_TEST_TARGET} PRIVATE
    BUILD_TESTING_MODULE_EVSE_MANAGER
)

target_link_libraries(${UTIL_TEST_TARGET}
    PRIVATE
        Catch2::Catch2WithMain
)
