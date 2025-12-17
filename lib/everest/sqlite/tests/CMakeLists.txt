set(TEST_TARGET_NAME ${PROJECT_NAME}_tests)
set(GTEST_LIBRARIES GTest::gmock_main GTest::gtest_main)

add_executable(${TEST_TARGET_NAME})

target_sources(${TEST_TARGET_NAME} PRIVATE
    test_database_schema_updater.cpp
    test_sqlite_statement.cpp
)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    everest::sqlite
    ${GTEST_LIBRARIES}
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

if (EVEREST_SQLITE_BUILD_TESTING AND NOT DISABLE_EDM)
    evc_include(CodeCoverage)
    append_coverage_compiler_flags_to_target(everest_sqlite)

    setup_target_for_coverage_gcovr_html(
        NAME ${PROJECT_NAME}_gcovr_coverage
        EXECUTABLE ctest
        DEPENDENCIES ${TEST_TARGET_NAME}
        EXCLUDE "tests/*"
    )

    setup_target_for_coverage_gcovr_xml(
        NAME ${PROJECT_NAME}_gcovr_coverage_xml
        EXECUTABLE ctest
        DEPENDENCIES ${TEST_TARGET_NAME}
        EXCLUDE "tests/*"
    )
endif()
