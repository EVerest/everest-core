include(GoogleTest)

file(GLOB_RECURSE FUSION_CHARGER_DISPENSER_LIB_TEST_SOURCES "*.cpp")

add_executable(dispenser-lib-tests ${FUSION_CHARGER_DISPENSER_LIB_TEST_SOURCES})
target_link_libraries(dispenser-lib-tests PRIVATE gtest_main fusion_charger_dispenser)

gtest_discover_tests(dispenser-lib-tests)
