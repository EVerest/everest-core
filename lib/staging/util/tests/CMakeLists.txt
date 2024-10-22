add_executable(EnumFlagsTest
    EnumFlagsTest.cpp
)

target_link_libraries(EnumFlagsTest
    PRIVATE
        GTest::gtest_main
        everest::staging::util
)

include(GoogleTest)
gtest_discover_tests(EnumFlagsTest)
