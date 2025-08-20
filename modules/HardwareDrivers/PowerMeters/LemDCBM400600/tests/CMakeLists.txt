set(TEST_TARGET_NAME ${PROJECT_NAME}_lem_dcbm_400600_controller_tests)

set(MODULE_DIR "${PROJECT_SOURCE_DIR}/modules/HardwareDrivers/PowerMeters/LemDCBM400600")

add_executable(${TEST_TARGET_NAME}
        test_lem_dcbm_400600_controller.cpp
        "${MODULE_DIR}/main/lem_dcbm_400600_controller.cpp"
        "${MODULE_DIR}/main/lem_dcbm_time_sync_helper.cpp"
)
add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

set(INCLUDE_DIR
        "main"
        "tests"
        "${MODULE_DIR}/main"
        "${MODULE_DIR}/tests")

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
        tests
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)

#
target_link_libraries(${TEST_TARGET_NAME} PRIVATE
        GTest::gmock_main
        everest::timer
        everest::framework
        nlohmann_json::nlohmann_json
        CURL::libcurl
)


add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})

## Time sync helper test
set(TEST_TARGET_NAME ${PROJECT_NAME}_lem_time_sync_helper_tests)
add_executable(${TEST_TARGET_NAME}
        test_lem_dcbm_time_sync_helper.cpp
        "${MODULE_DIR}/main/lem_dcbm_time_sync_helper.cpp"
)

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

set(INCLUDE_DIR
        "main"
        "tests"
        "${MODULE_DIR}/main"
        "${MODULE_DIR}/tests")

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
        tests
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)

#
target_link_libraries(${TEST_TARGET_NAME} PRIVATE
        GTest::gmock_main
        everest::timer
        everest::framework
        nlohmann_json::nlohmann_json
        CURL::libcurl
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})


## http client integration client

add_executable(integration_test_http_client
        integration_test_http_client.cpp
        "${MODULE_DIR}/main/http_client.cpp"
)

target_include_directories(integration_test_http_client PRIVATE
        tests
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)


target_link_libraries(integration_test_http_client PRIVATE
        GTest::gmock_main
        everest::timer
        everest::framework
        nlohmann_json::nlohmann_json
        CURL::libcurl
)
