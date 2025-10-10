add_executable(everest_util_tests
    enum/EnumFlagsTest.cpp
    enum/EnumFlagsTest_B.cpp
    async/monitor_tests.cpp
)

target_link_libraries(everest_util_tests
    PRIVATE
        GTest::gtest_main
        everest::util
)

include(GoogleTest)
gtest_discover_tests(everest_util_tests)
