// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <lib/ocpp/common/test_database_migration_files.hpp>

// Apply generic test cases to v201 migrations
INSTANTIATE_TEST_SUITE_P(V201, DatabaseMigrationFilesTest,
                         ::testing::Values(std::make_tuple(std::filesystem::path(MIGRATION_FILES_LOCATION_V201),
                                                           MIGRATION_FILE_VERSION_V201)));
