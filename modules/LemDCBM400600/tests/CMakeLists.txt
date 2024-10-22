set(TEST_TARGET_NAME ${PROJECT_NAME}_lem_dcbm_400600_controller_tests)
add_executable(${TEST_TARGET_NAME}
        test_lem_dcbm_400600_controller.cpp
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main/lem_dcbm_400600_controller.cpp"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main/lem_dcbm_time_sync_helper.cpp"
)
add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

set(INCLUDE_DIR
        "main"
        "tests"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/tests")

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

## Time sync helper test
set(TEST_TARGET_NAME ${PROJECT_NAME}_lem_time_sync_helper_tests)
add_executable(${TEST_TARGET_NAME}
        test_lem_dcbm_time_sync_helper.cpp
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main/lem_dcbm_time_sync_helper.cpp"
)

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

set(INCLUDE_DIR
        "main"
        "tests"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/tests")

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


## http client integration client

add_executable(integration_test_http_client
        integration_test_http_client.cpp
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main/http_client.cpp"
)

target_include_directories(integration_test_http_client PRIVATE
        tests
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)

#
target_link_libraries(integration_test_http_client PRIVATE
        GTest::gmock_main
        everest::timer
        everest::framework
        nlohmann_json::nlohmann_json
        CURL::libcurl
)
