set(TEST_TARGET_NAME ${PROJECT_NAME}_${MODULE_NAME}_tests)

add_executable(${TEST_TARGET_NAME})

target_sources(${TEST_TARGET_NAME} PRIVATE
    ../VirtualPaymentTerminal/VirtualPT.cpp
    ../PaymentTerminalDriver/PTDriver.cpp
    VirtualPT_test.cpp
    PTDriver_test.cpp
)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
    ..
#    ${GENERATED_INCLUDE_DIR}
#    ${PERSISTENT_STORE_MODULE_DIR}/main
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
#    everest::framework
)
add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
