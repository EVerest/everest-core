include(GoogleTest)

file(GLOB_RECURSE GOOSE_ETHERNET_TEST_SOURCES "*.cpp")
add_executable(goose-ethernet-tests ${GOOSE_ETHERNET_TEST_SOURCES})
target_link_libraries(goose-ethernet-tests PRIVATE goose-ethernet gtest_main)
gtest_discover_tests(goose-ethernet-tests)
