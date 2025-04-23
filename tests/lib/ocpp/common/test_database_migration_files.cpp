// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "test_database_migration_files.hpp"

TEST_P(DatabaseMigrationFilesTest, ApplyMigrationFilesStepByStep) {
    everest::db::sqlite::SchemaUpdater updater{this->database.get()};

    for (uint32_t i = 1; i <= this->max_version; i++) {
        EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, i));
        this->ExpectUserVersion(i);
    }

    for (uint32_t i = this->max_version; i > 0; i--) {
        EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, i));
        this->ExpectUserVersion(i);
    }
}

TEST_P(DatabaseMigrationFilesTest, ApplyMigrationFilesAtOnce) {
    everest::db::sqlite::SchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, this->max_version));
    this->ExpectUserVersion(this->max_version);

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);
}