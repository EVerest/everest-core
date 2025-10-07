include(GoogleTest)

file(GLOB_RECURSE GOOSE_TEST_SOURCES "*.cpp")
add_executable(goose-tests ${GOOSE_TEST_SOURCES})
target_link_libraries(goose-tests goose gtest_main)
gtest_discover_tests(goose-tests EXTRA_ARGS "--gtest_repeat=3")
