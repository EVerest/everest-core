if(NOT DISABLE_EDM)
    list(APPEND CMAKE_MODULE_PATH ${CPM_PACKAGE_catch2_SOURCE_DIR}/extras)
endif()
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

include(Catch)

set(TEST_TARGET_NAME ${PROJECT_NAME}_tests)
add_executable(${TEST_TARGET_NAME})

target_include_directories(${TEST_TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(${TEST_TARGET_NAME} PRIVATE
    test_config.cpp
    test_config_sqlite.cpp
    test_conversions.cpp
    test_filesystem_helpers.cpp
    helpers.cpp
)

add_subdirectory(controller)

target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
        everest::framework
        everest::log
        everest::sqlite
        Catch2::Catch2WithMain
)

catch_discover_tests(${TEST_TARGET_NAME})

include(test_utilities.cmake)

setup_test_directory(empty_config)
setup_test_directory(valid_config)
setup_test_directory(valid_module_config TESTValidManifest test_interface)
setup_test_directory(valid_module_config_userconfig TESTValidManifest test_interface
    CONFIG valid_module_config_config.yaml
    USER_CONFIG valid_module_config_userconfig.yaml
)
setup_test_directory(valid_module_config_validate TESTValidManifestCmdVar test_interface_cmd_var
    TYPE_FILES test_type.yaml) # FIXME (aw): type is missing
setup_test_directory(valid_module_config_json TESTValidManifest test_interface
    CONFIG valid_module_config_json_config.json
)

setup_test_directory(broken_yaml)
setup_test_directory(empty_yaml_object USE_FILESYSTEM_HIERARCHY_STANDARD)
setup_test_directory(empty_yaml USE_FILESYSTEM_HIERARCHY_STANDARD)
setup_test_directory(null_yaml USE_FILESYSTEM_HIERARCHY_STANDARD)
setup_test_directory(string_yaml USE_FILESYSTEM_HIERARCHY_STANDARD)

setup_test_directory(missing_module)
setup_test_directory(broken_manifest_1 TESTBrokenManifest1 test_interface)
setup_test_directory(broken_manifest_2 TESTBrokenManifest2 test_interface)
setup_test_directory(broken_manifest_3 TESTBrokenManifest3 test_interface)
setup_test_directory(broken_manifest_4 TESTBrokenManifest4 test_interface)
setup_test_directory(missing_interface TESTMissingInterface)

setup_test_directory(unknown_impls TESTValidManifest test_interface)
setup_test_directory(missing_config_entry TESTValidManifest test_interface)
setup_test_directory(invalid_config_entry_type TESTValidManifest test_interface)
setup_test_directory(missing_impl_config_entry TESTValidManifest test_interface)

setup_test_directory(
    valid_complete_config
    CONFIG valid_complete_config.json
    MODULES TESTValidManifest TESTValidManifestCmdVar TESTValidManifestRequires
    INTERFACE_FILES test_interface test_interface_cmd_var
)

setup_test_directory(
    two_module_test 
    CONFIG two_modules.yaml 
    MODULES TESTModuleA TESTModuleB
    INTERFACE_FILES test_interface test_interface_cmd_var
)

file(COPY ${PROJECT_SOURCE_DIR}/schemas/migrations
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

evc_include(CodeCoverage)

append_coverage_compiler_flags_to_target(framework)
append_coverage_compiler_flags_to_target(manager)
append_coverage_compiler_flags_to_target(controller)

# set(GCOVR_ADDITIONAL_ARGS "--gcov-ignore-errors=all")
setup_target_for_coverage_gcovr_html(
    NAME ${PROJECT_NAME}_gcovr_coverage
    EXECUTABLE ctest --output-on-failure
    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
    DEPENDENCIES ${PROJECT_NAME}_tests everest::framework
    EXCLUDE "${CMAKE_BINARY_DIR}/*" "${CPM_SOURCE_CACHE}"
)

setup_target_for_coverage_gcovr_xml(
    NAME ${PROJECT_NAME}_gcovr_coverage_xml
    EXECUTABLE ctest --output-on-failure
    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
    DEPENDENCIES ${PROJECT_NAME}_tests everest::framework
    EXCLUDE "${CMAKE_BINARY_DIR}/*" "${CPM_SOURCE_CACHE}"
)
