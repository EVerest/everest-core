// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <lib/ocpp/common/test_database_migration_files.hpp>

// Apply generic test cases to v16 migrations
INSTANTIATE_TEST_SUITE_P(V16, DatabaseMigrationFilesTest,
                         ::testing::Values(std::make_tuple(std::filesystem::path(MIGRATION_FILES_LOCATION_V16),
                                                           MIGRATION_FILE_VERSION_V16)));

// Apply v16 specific test cases to migrations
using DatabaseMigrationFilesTestV16 = DatabaseMigrationFilesTest;

INSTANTIATE_TEST_SUITE_P(V16, DatabaseMigrationFilesTestV16,
                         ::testing::Values(std::make_tuple(std::filesystem::path(MIGRATION_FILES_LOCATION_V16),
                                                           MIGRATION_FILE_VERSION_V16)));

TEST_P(DatabaseMigrationFilesTestV16, V16_MigrationFile2) {
    everest::db::sqlite::SchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);

    // Transaction table should not contain these columns yet
    EXPECT_FALSE(this->DoesColumnExist("TRANSACTIONS", "START_TRANSACTION_MESSAGE_ID"));
    EXPECT_FALSE(this->DoesColumnExist("TRANSACTIONS", "STOP_TRANSACTION_MESSAGE_ID"));

    // We expect to be able to insert into CONNECTORS and TRANSACTIONS table.
    EXPECT_TRUE(this->database->execute_statement("INSERT INTO CONNECTORS (ID, AVAILABILITY) VALUES (1, \"\")"));

    std::string sql =
        "INSERT INTO TRANSACTIONS "
        "(ID, CONNECTOR, ID_TAG_START, TIME_START, METER_START, CSMS_ACK, METER_LAST, METER_LAST_TIME, LAST_UPDATE)"
        " VALUES "
        "(55, 1,         \"\",         \"\",       1,           0,        0,          \"\",            \"\")";
    EXPECT_TRUE(this->database->execute_statement(sql));

    // We added a row with CSMS_ACK=0 so we should not find anything
    auto stmt = this->database->new_statement("SELECT ID FROM TRANSACTIONS WHERE CSMS_ACK=1;");
    EXPECT_EQ(stmt->step(), SQLITE_DONE);

    // After applying the migration we expect to be at version 2
    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 2));
    this->ExpectUserVersion(2);

    // We expect the added columns to exists
    EXPECT_TRUE(this->DoesColumnExist("TRANSACTIONS", "START_TRANSACTION_MESSAGE_ID"));
    EXPECT_TRUE(this->DoesColumnExist("TRANSACTIONS", "STOP_TRANSACTION_MESSAGE_ID"));

    // We should be able to update the transaction we inserted earlier
    EXPECT_TRUE(this->database->execute_statement("UPDATE TRANSACTIONS SET METER_LAST=2 WHERE ID=55"));
    // We should be able to update the newly introduced field too
    EXPECT_TRUE(this->database->execute_statement(
        "UPDATE TRANSACTIONS SET START_TRANSACTION_MESSAGE_ID=\"test2\" WHERE ID=55"));

    // The migration should have set all rows to CSMS_ACK=1 so we should find 1 row with ID=55 here
    stmt = this->database->new_statement("SELECT ID FROM TRANSACTIONS WHERE CSMS_ACK=1;");
    EXPECT_EQ(stmt->step(), SQLITE_ROW);
    EXPECT_EQ(stmt->column_int(0), 55);
    EXPECT_EQ(stmt->step(), SQLITE_DONE);

    // After applying the down migration we expect to be at version 1
    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);

    // We expect the added columns to no longer exists
    EXPECT_FALSE(this->DoesColumnExist("TRANSACTIONS", "START_TRANSACTION_MESSAGE_ID"));
    EXPECT_FALSE(this->DoesColumnExist("TRANSACTIONS", "STOP_TRANSACTION_MESSAGE_ID"));

    // We should still be able to update the transaction we inserted earlier
    EXPECT_TRUE(this->database->execute_statement("UPDATE TRANSACTIONS SET METER_LAST=2 WHERE ID=55"));
    // We should not be able to update the field from version 2 any longer
    EXPECT_FALSE(this->database->execute_statement(
        "UPDATE TRANSACTIONS SET START_TRANSACTION_MESSAGE_ID=\"test2\" WHERE ID=55"));

    // The down migration should not have touched CSMS_ACK=1 so we should still find 1 row with ID=55 here
    stmt = this->database->new_statement("SELECT ID FROM TRANSACTIONS WHERE CSMS_ACK=1;");
    EXPECT_EQ(stmt->step(), SQLITE_ROW);
    EXPECT_EQ(stmt->column_int(0), 55);
    EXPECT_EQ(stmt->step(), SQLITE_DONE);
}