set (TRANSPILE_CONFIG_TEST "${TEST_TARGET_NAME}_transpile_config")

add_executable(
    ${TRANSPILE_CONFIG_TEST}
        test_transpile_config.cpp
        ${PROJECT_SOURCE_DIR}/src/controller/transpile_config.cpp
)

target_include_directories(${TRANSPILE_CONFIG_TEST}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/controller
)

target_link_libraries(${TRANSPILE_CONFIG_TEST}
    PRIVATE
        Catch2::Catch2WithMain

        nlohmann_json::nlohmann_json
        ryml::ryml
)

catch_discover_tests(${TRANSPILE_CONFIG_TEST})
