// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <lib/ocpp/common/test_database_migration_files.hpp>

// Apply generic test cases to v2 migrations
INSTANTIATE_TEST_SUITE_P(V2, DatabaseMigrationFilesTest,
                         ::testing::Values(std::make_tuple(std::filesystem::path(MIGRATION_FILES_LOCATION_V2),
                                                           MIGRATION_FILE_VERSION_V2)));

// Apply v2 specific test cases to migrations
using DatabaseMigrationFilesTestV2 = DatabaseMigrationFilesTest;

INSTANTIATE_TEST_SUITE_P(V2, DatabaseMigrationFilesTestV2,
                         ::testing::Values(std::make_tuple(std::filesystem::path(MIGRATION_FILES_LOCATION_V2),
                                                           MIGRATION_FILE_VERSION_V2)));

TEST_P(DatabaseMigrationFilesTestV2, V2_MigrationFile2_AuthCacheManagement) {
    DatabaseSchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);

    // Transaction table should not contain these columns yet
    EXPECT_FALSE(this->DoesColumnExist("AUTH_CACHE", "LAST_USED"));
    EXPECT_FALSE(this->DoesColumnExist("AUTH_CACHE", "EXPIRY_DATE"));

    // We expect to be able to insert into AUTH_CACHE table.
    EXPECT_TRUE(this->database->execute_statement(
        "INSERT INTO AUTH_CACHE (ID_TOKEN_HASH, ID_TOKEN_INFO) VALUES (\"hash\", \"info\")"));

    // After applying the migration we expect to be at version 2
    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 2));
    this->ExpectUserVersion(2);

    // We expect the added columns to exists
    EXPECT_TRUE(this->DoesColumnExist("AUTH_CACHE", "LAST_USED"));
    EXPECT_TRUE(this->DoesColumnExist("AUTH_CACHE", "EXPIRY_DATE"));

    // We should be able to update the cache item we inserted earlier
    EXPECT_TRUE(this->database->execute_statement(
        "UPDATE AUTH_CACHE SET ID_TOKEN_INFO=\"info2\" WHERE ID_TOKEN_HASH=\"hash\""));
    // We should be able to update the newly introduced field too
    EXPECT_TRUE(
        this->database->execute_statement("UPDATE AUTH_CACHE SET EXPIRY_DATE=9870 WHERE ID_TOKEN_HASH=\"hash\""));

    // The migration should have initialized all rows to LAST_USED=0 so we should find 1 row with with that value here
    auto stmt = this->database->new_statement("SELECT LAST_USED FROM AUTH_CACHE WHERE ID_TOKEN_HASH=\"hash\";");
    EXPECT_EQ(stmt->step(), SQLITE_ROW);
    EXPECT_EQ(stmt->column_int(0), 0);
    EXPECT_EQ(stmt->step(), SQLITE_DONE);

    // After applying the down migration we expect to be at version 1
    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);

    // We expect the added columns to no longer exists
    EXPECT_FALSE(this->DoesColumnExist("AUTH_CACHE", "LAST_USED"));
    EXPECT_FALSE(this->DoesColumnExist("AUTH_CACHE", "EXPIRY_DATE"));

    // The migration should have left all rows so we should find 1 row with ID_TOKEN_HASH="hash" here
    stmt = this->database->new_statement("SELECT ID_TOKEN_INFO FROM AUTH_CACHE WHERE ID_TOKEN_HASH=\"hash\";");
    EXPECT_EQ(stmt->step(), SQLITE_ROW);
    EXPECT_EQ(stmt->column_text(0), "info2");
    EXPECT_EQ(stmt->step(), SQLITE_DONE);
}