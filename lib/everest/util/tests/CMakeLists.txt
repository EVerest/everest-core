add_executable(EnumFlagsTest
    enum/EnumFlagsTest.cpp
    enum/EnumFlagsTest_B.cpp
)

target_link_libraries(EnumFlagsTest
    PRIVATE
        GTest::gtest_main
        everest::util
)

include(GoogleTest)
gtest_discover_tests(EnumFlagsTest)
