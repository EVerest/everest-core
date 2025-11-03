find_package(GTest REQUIRED)
add_executable(${PROJECT_NAME}_unit_test libslac_unit_test.cpp)

target_include_directories(${PROJECT_NAME}_unit_test PUBLIC${GTEST_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}_unit_test PRIVATE
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
)

add_test(${PROJECT_NAME}_unit_test ${PROJECT_NAME}_unit_test)
