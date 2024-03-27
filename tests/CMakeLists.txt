find_package(GTest REQUIRED)

set(TEST_TARGET_NAME ${PROJECT_NAME}_tests)
add_executable(${TEST_TARGET_NAME} liblog_test.cpp)

target_include_directories(${TEST_TARGET_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})
target_include_directories(log
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${TEST_TARGET_NAME} PRIVATE
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
)

gtest_discover_tests(${TEST_TARGET_NAME})
