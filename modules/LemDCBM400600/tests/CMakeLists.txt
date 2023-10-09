
add_executable(test_lem_dcbm_400600_controller
        test_lem_dcbm_400600_controller.cpp
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main/lem_dcbm_400600_controller.cpp"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main/lem_dcbm_time_sync_helper.cpp"
)

set(INCLUDE_DIR
        "main"
        "tests"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/tests")

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

find_package(GTest REQUIRED)

target_include_directories(test_lem_dcbm_400600_controller PRIVATE
        tests
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)

#
target_link_libraries(test_lem_dcbm_400600_controller PRIVATE
        GTest::gmock_main
        everest::timer
        everest::framework
        nlohmann_json::nlohmann_json
        CURL::libcurl
)


add_test(test_lem_dcbm_400600_controller test_lem_dcbm_400600_controller)


## Time sync helper test
add_executable(test_lem_time_sync_helper
        test_lem_dcbm_time_sync_helper.cpp
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main/lem_dcbm_time_sync_helper.cpp"
)

set(INCLUDE_DIR
        "main"
        "tests"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/main"
        "${PROJECT_SOURCE_DIR}/modules/LemDCBM400600/tests")

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

find_package(GTest REQUIRED)

target_include_directories(test_lem_time_sync_helper PRIVATE
        tests
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)

#
target_link_libraries(test_lem_time_sync_helper PRIVATE
        GTest::gmock_main
        everest::timer
        everest::framework
        nlohmann_json::nlohmann_json
        CURL::libcurl
)

add_test(test_lem_dcbm_time_sync_helper test_lem_dcbm_time_sync_helper)


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
