set(TEST_TARGET_NAME ${PROJECT_NAME}_carlo_gavazzi_em580_helper_tests)

set(MODULE_DIR "${PROJECT_SOURCE_DIR}/modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580")

add_executable(${TEST_TARGET_NAME}
        test_em580_helper.cpp
        test_em580_powermeter_impl.cpp
        ${MODULE_DIR}/main/powermeterImpl.cpp
        ${MODULE_DIR}/main/transport.cpp
)

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

set(INCLUDE_DIR
        "main"
        "tests"
        "${MODULE_DIR}/main"
        "${MODULE_DIR}/tests"
)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
        tests
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/generated/modules/CarloGavazzi_EM580
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
        GTest::gtest_main
        everest::framework
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})


