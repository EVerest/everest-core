add_executable(${PROJECT_NAME}_unit_test libslac_unit_test.cpp)

target_include_directories(${PROJECT_NAME}_unit_test PUBLIC${GTEST_INCLUDE_DIRS})


if(DISABLE_EDM)
    find_package(GTest REQUIRED)
else()
    set(GTEST_LIBRARIES
        GTest::gtest
        GTest::gtest_main
    )
endif()

target_link_libraries(${PROJECT_NAME}_unit_test PRIVATE
        ${GTEST_LIBRARIES}
        slac::slac
)

add_test(${PROJECT_NAME}_unit_test ${PROJECT_NAME}_unit_test)
