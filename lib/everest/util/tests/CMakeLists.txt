add_executable(everest_util_tests
  async/monitor_tests.cpp
  async/thread_pool_tests.cpp
  async/wrapper_tests.cpp
  enum/EnumFlagsTest.cpp
  enum/EnumFlagsTest_B.cpp
  queue/simple_queue_tests.cpp
  queue/thread_safe_queue_tests.cpp
)

target_link_libraries(everest_util_tests
  PRIVATE
        GTest::gtest_main
        everest::util
)

include(GoogleTest)
gtest_discover_tests(everest_util_tests)
