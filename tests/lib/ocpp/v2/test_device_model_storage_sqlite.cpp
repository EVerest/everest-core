// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <device_model_test_helper.hpp>

#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/device_model_storage_sqlite.hpp>
#include <ocpp/v2/init_device_model_db.hpp>

namespace ocpp {
namespace v2 {

class DeviceModelStorageSQLiteTest : public ::testing::Test {
protected:
    const std::string DATABASE_PATH = "file::memory:?cache=shared";
    const std::string MIGRATION_FILES_PATH = "./resources/v2/device_model_migration_files";
    const std::string CONFIGS_PATH = "./resources/config/v2/component_config";
    DeviceModelTestHelper device_model_test_helper;

public:
    DeviceModelStorageSQLiteTest() : device_model_test_helper(DATABASE_PATH, MIGRATION_FILES_PATH, CONFIGS_PATH) {
    }
};

/// \brief Tests check_integrity does not raise error for valid database
TEST_F(DeviceModelStorageSQLiteTest, test_check_integrity_valid) {
    DeviceModelStorageSqlite dm(DATABASE_PATH, "", "", false);

    EXPECT_NO_THROW(dm.check_integrity());
}

/// \brief Tests check_integrity raises exception for invalid database
TEST_F(DeviceModelStorageSQLiteTest, test_check_integrity_invalid) {

    device_model_test_helper.remove_variable_from_db("DisplayMessageCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                                     "NumberOfDisplayMessages", std::nullopt);

    DeviceModelStorageSqlite dm(DATABASE_PATH, "", "", false);

    EXPECT_NO_THROW(dm.check_integrity());
}

} // namespace v2
} // namespace ocpp
