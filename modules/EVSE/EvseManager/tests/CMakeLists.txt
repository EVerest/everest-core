set(TEST_TARGET_NAME ${PROJECT_NAME}_EvseManager_tests)

set(TESTS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/tests/include")

add_executable(${TEST_TARGET_NAME})

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
    .. ${TESTS_INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
)

target_sources(${TEST_TARGET_NAME} PRIVATE
    EventQueueTest.cpp
    ErrorHandlingTest.cpp
    IECStateMachineTest.cpp
    OverVoltageMonitorTest.cpp
    VoltagePlausibilityMonitorTest.cpp
    ../ErrorHandling.cpp
    ../IECStateMachine.cpp
    ../backtrace.cpp
    ../over_voltage/OverVoltageMonitor.cpp
    ../voltage_plausibility/VoltagePlausibilityMonitor.cpp
)

target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
    BUILD_TESTING_MODULE_EVSE_MANAGER
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
    everest::log
    everest::framework
    everest::helpers
    sigslot
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})

set(CHARGER_TEST_TARGET_NAME ${PROJECT_NAME}_EvseManagerCharger_tests)
add_executable(${CHARGER_TEST_TARGET_NAME})

add_dependencies(${CHARGER_TEST_TARGET_NAME} ${MODULE_NAME})

target_include_directories(${CHARGER_TEST_TARGET_NAME} PRIVATE
    .. ${TESTS_INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
)

target_sources(${CHARGER_TEST_TARGET_NAME} PRIVATE
    ChargerTest.cpp
    ../Charger.cpp
)

target_compile_definitions(${CHARGER_TEST_TARGET_NAME} PRIVATE
    BUILD_TESTING_MODULE_EVSE_MANAGER
)

target_link_libraries(${CHARGER_TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
    everest::framework
    everest::helpers
    sigslot
)

add_test(${CHARGER_TEST_TARGET_NAME} ${CHARGER_TEST_TARGET_NAME})
ev_register_test_target(${CHARGER_TEST_TARGET_NAME})
