set(TEST_TARGET_NAME ${PROJECT_NAME}_tests)
set(GTEST_LIBRARIES GTest::gmock_main GTest::gtest_main)

add_executable(${TEST_TARGET_NAME} liblog_test.cpp)

target_include_directories(${TEST_TARGET_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})
target_include_directories(log
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
        everest::log
        ${GTEST_LIBRARIES}
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

if (EVEREST_LIBLOG_BUILD_TESTING)
    evc_include(CodeCoverage)
    append_coverage_compiler_flags_to_target(log)

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
