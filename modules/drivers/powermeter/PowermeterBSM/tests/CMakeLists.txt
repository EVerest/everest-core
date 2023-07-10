# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

add_executable( test_rtu_device test_rtu_device.cpp test_helper.cpp )
target_link_libraries( test_rtu_device
  sunspec_framework_object_lib
  GTest::gtest_main
  GTest::gmock
  )

add_executable( test_snapshot_models test_snapshot_models.cpp )
target_link_libraries( test_snapshot_models
  sunspec_framework_object_lib
  GTest::gtest_main
  GTest::gmock
  )
