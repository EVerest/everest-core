// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include "database_testing_utils.hpp"
#include <everest/database/sqlite/schema_updater.hpp>

class DatabaseMigrationFilesTest : public DatabaseTestingUtils,
                                   public ::testing::WithParamInterface<std::tuple<std::filesystem::path, uint32_t>> {

protected:
    const std::filesystem::path migration_files_path;
    const uint32_t max_version;

public:
    DatabaseMigrationFilesTest() :
        DatabaseTestingUtils(),
        migration_files_path(std::get<std::filesystem::path>(GetParam())),
        max_version(std::get<uint32_t>(GetParam())) {
        EXPECT_TRUE(this->database->open_connection());
    }
};
