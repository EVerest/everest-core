add_executable(timer_test)
target_sources(timer_test
  PRIVATE
      "../Timer/SimpleTimeout.cpp"
      "Timer_ManualTest.cpp")
target_include_directories(timer_test
  PRIVATE
      ".."
      ${everest-framework_SOURCE_DIR})

