// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ocpp/v201/device_model_storage_sqlite.hpp>

namespace ocpp {
namespace v201 {

class DeviceModelStorageSQLiteTest : public ::testing::Test {
protected:
    const std::string DEVICE_MODEL_DATABASE = "./resources/unittest_device_model.db";
    const std::string INVALID_DEVICE_MODEL_DATABASE = "./resources/unittest_device_model_missing_required.db";
};

/// \brief Tests check_integrity does not raise error for valid database
TEST_F(DeviceModelStorageSQLiteTest, test_check_integrity_valid) {

    auto dm_storage = DeviceModelStorageSqlite(DEVICE_MODEL_DATABASE);
    dm_storage.check_integrity();
}

/// \brief Tests check_integrity raises exception for invalid database
TEST_F(DeviceModelStorageSQLiteTest, test_check_integrity_invalid) {

    auto dm_storage = DeviceModelStorageSqlite(INVALID_DEVICE_MODEL_DATABASE);
    EXPECT_THROW(dm_storage.check_integrity(), DeviceModelError);
}

} // namespace v201
} // namespace ocpp
