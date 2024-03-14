# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
set(TEST_TARGET_NAME ${PROJECT_NAME}_test_rtu_device)
add_executable( ${TEST_TARGET_NAME} test_rtu_device.cpp test_helper.cpp )
target_link_libraries( ${TEST_TARGET_NAME}
  sunspec_framework_object_lib
  GTest::gtest_main
  GTest::gmock
  )
add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

set(TEST_TARGET_NAME ${PROJECT_NAME}_test_snapshot_models)
add_executable( ${TEST_TARGET_NAME} test_snapshot_models.cpp )
target_link_libraries( ${TEST_TARGET_NAME}
  sunspec_framework_object_lib
  GTest::gtest_main
  GTest::gmock
  )
add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
