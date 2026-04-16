set(TEST_TARGET_NAME ${PROJECT_NAME}_OCPPExtensionExample_tests)
set(TESTS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/tests/include")

add_executable(${TEST_TARGET_NAME})
add_dependencies(${TEST_TARGET_NAME} ld-ev_${MODULE_NAME})
set_source_files_properties("${MODULE_LOADER_DIR}/ld-ev.cpp" PROPERTIES GENERATED TRUE)
get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)
target_include_directories(${TEST_TARGET_NAME} PRIVATE
    .. ../data_transfer ${TESTS_INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
)
target_sources(${TEST_TARGET_NAME} PRIVATE
    test_ocpp_extension_example.cpp
    ../data_transfer/ocpp_data_transferImpl.cpp
    ../OCPPExtensionExample.cpp
    # auto generated module loader
    "${MODULE_LOADER_DIR}/ld-ev.cpp"
)
target_compile_definitions(${TEST_TARGET_NAME} PRIVATE LD_EV_EXCLUDE_MAIN)
target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gtest_main
    everest::log
    everest::framework
    everest::helpers
    sigslot
)
add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})

# The simple test
set(TEST_TARGET_NAME ${PROJECT_NAME}_OCPPExtensionSimple_test)
add_executable(${TEST_TARGET_NAME})
add_dependencies(${TEST_TARGET_NAME} ld-ev_${MODULE_NAME})
set_source_files_properties("${MODULE_LOADER_DIR}/ld-ev.cpp" PROPERTIES GENERATED TRUE)
get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)
target_include_directories(${TEST_TARGET_NAME} PRIVATE
    .. ../data_transfer ${TESTS_INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
)
target_sources(${TEST_TARGET_NAME} PRIVATE
    test_simple.cpp
    ../data_transfer/ocpp_data_transferImpl.cpp
    ../OCPPExtensionExample.cpp
    # auto generated module loader
    "${MODULE_LOADER_DIR}/ld-ev.cpp"
)
target_compile_definitions(${TEST_TARGET_NAME} PRIVATE LD_EV_EXCLUDE_MAIN)
target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gtest_main
    everest::log
    everest::framework
    everest::helpers
    sigslot
)
add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
