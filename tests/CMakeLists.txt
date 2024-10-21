add_subdirectory(everest-core_tests)

setup_target_for_coverage_gcovr_html(
    NAME ${PROJECT_NAME}_gcovr_coverage_html
    EXECUTABLE ctest
    DEPENDENCIES ${PROJECT_NAME}_unit_tests
)

setup_target_for_coverage_gcovr_xml(
    NAME ${PROJECT_NAME}_gcovr_coverage_xml
    EXECUTABLE ctest
    DEPENDENCIES ${PROJECT_NAME}_unit_tests
)
