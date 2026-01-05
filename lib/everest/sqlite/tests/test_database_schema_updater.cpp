// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "database_testing_utils.hpp"
#include <everest/database/sqlite/schema_updater.hpp>
#include <fstream>

namespace everest::db::sqlite {

struct MigrationFile {
    std::string_view name;
    std::string_view content;
};

static constexpr MigrationFile migration_file_up_1_valid{
    "1_up-initial.sql",
    "PRAGMA foreign_keys = ON; CREATE TABLE TEST_TABLE1(FIELD1 TEXT PRIMARY KEY NOT NULL, FIELD2 INT NOT NULL);"};

static constexpr MigrationFile migration_file_up_1_valid_empty_name{
    "1_up.sql",
    "PRAGMA foreign_keys = ON; CREATE TABLE TEST_TABLE1(FIELD1 TEXT PRIMARY KEY NOT NULL, FIELD2 INT NOT NULL);"};

static constexpr MigrationFile migration_file_up_1_invalid{
    "1_up-initial.sql", "PRAGMA foreign_keys = ON; CREATE TABLE <invalid> TEST_TABLE1(FIELD1 TEXT PRIMARY KEY NOT "
                        "NULL, FIELD2 INT NOT NULL);"};

static constexpr MigrationFile migration_file_up_2_valid{
    "2_up-add_table.sql", "CREATE TABLE TEST_TABLE2(FIELD1 TEXT PRIMARY KEY NOT NULL, FIELD2 INT NOT NULL);"};

static constexpr MigrationFile migration_file_down_2_valid{"2_down-drop_table.sql", "DROP TABLE TEST_TABLE2;"};

static constexpr MigrationFile migration_file_up_3_valid{
    "3_up-add_table.sql", "CREATE TABLE TEST_TABLE3(FIELD1 TEXT PRIMARY KEY NOT NULL, FIELD2 INT NOT NULL);"};

static constexpr MigrationFile migration_file_down_3_valid{"3_down-drop_table.sql", "DROP TABLE TEST_TABLE3;"};

static constexpr MigrationFile migration_file_up_4_valid{
    "4_up-add_table.sql", "CREATE TABLE TEST_TABLE4(FIELD1 TEXT PRIMARY KEY NOT NULL, FIELD2 INT NOT NULL);"};

static constexpr MigrationFile migration_file_down_4_valid{"4_down-drop_table.sql", "DROP TABLE TEST_TABLE4;"};

static constexpr std::string_view table1{"TEST_TABLE1"};
static constexpr std::string_view table2{"TEST_TABLE2"};
static constexpr std::string_view table3{"TEST_TABLE3"};
static constexpr std::string_view table4{"TEST_TABLE3"};

class DatabaseSchemaUpdaterTest : public DatabaseTestingUtils {

protected:
    std::filesystem::path migration_files_path;

public:
    DatabaseSchemaUpdaterTest() :
        DatabaseTestingUtils(),
        migration_files_path(std::filesystem::temp_directory_path() / "database_schema_test" / "core_migrations") {
        std::filesystem::create_directories(migration_files_path);
        EXPECT_TRUE(this->database->open_connection());
    }

    ~DatabaseSchemaUpdaterTest() {
        std::filesystem::remove_all(migration_files_path);
    }

    void WriteMigrationFile(const MigrationFile& file) {
        std::ofstream stream{this->migration_files_path / file.name};
        stream << file.content;
    }
};

TEST_F(DatabaseSchemaUpdaterTest, FolderDoesNotExist) {
    SchemaUpdater updater{this->database.get()};
    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path / "invalid", 1));
}

TEST_F(DatabaseSchemaUpdaterTest, TargetVersionInvalid) {
    SchemaUpdater updater{this->database.get()};
    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 0));
}

TEST_F(DatabaseSchemaUpdaterTest, ApplyInitialMigrationFile) {

    this->WriteMigrationFile(migration_file_up_1_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));

    this->ExpectUserVersion(1);
    EXPECT_TRUE(this->DoesTableExist(table1));
}

TEST_F(DatabaseSchemaUpdaterTest, ApplyInitialMigrationFileEmptyName) {

    this->WriteMigrationFile(migration_file_up_1_valid_empty_name);

    SchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));

    this->ExpectUserVersion(1);
    EXPECT_TRUE(this->DoesTableExist(table1));
}

TEST_F(DatabaseSchemaUpdaterTest, ApplyInitialMigrationFileAlreadyUpToDate) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->SetUserVersion(1);

    SchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));

    this->ExpectUserVersion(1);
    EXPECT_FALSE(this->DoesTableExist(table1)); // Database was not changed
}

TEST_F(DatabaseSchemaUpdaterTest, ApplyInitialMigrationFileVersionToHigh) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->SetUserVersion(2);

    SchemaUpdater updater{this->database.get()};

    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 1));

    this->ExpectUserVersion(2);
    EXPECT_FALSE(this->DoesTableExist(table1)); // Database was not changed
}

TEST_F(DatabaseSchemaUpdaterTest, ApplyInvalidInitialMigrationFile) {

    this->WriteMigrationFile(migration_file_up_1_invalid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 1));

    this->ExpectUserVersion(0);
    EXPECT_FALSE(this->DoesTableExist(table1)); // Database was not changed
}

TEST_F(DatabaseSchemaUpdaterTest, MissingInitialMigrationFile) {

    this->WriteMigrationFile(migration_file_up_2_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 1));

    this->ExpectUserVersion(0);
    EXPECT_FALSE(this->DoesTableExist(table1));
}

TEST_F(DatabaseSchemaUpdaterTest, SequenceNotValidUnevenNrOfFiles) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->WriteMigrationFile(migration_file_up_2_valid);
    this->WriteMigrationFile(migration_file_up_3_valid);
    this->WriteMigrationFile(migration_file_down_3_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 1));
    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 2));
    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 3));

    this->ExpectUserVersion(0);
    EXPECT_FALSE(this->DoesTableExist(table1)); // Database was not changed
}

TEST_F(DatabaseSchemaUpdaterTest, SequenceNotValidNotEnoughFiles) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->WriteMigrationFile(migration_file_up_2_valid);
    this->WriteMigrationFile(migration_file_down_2_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 3));

    this->ExpectUserVersion(0);
    EXPECT_FALSE(this->DoesTableExist(table1)); // Database was not changed
}

TEST_F(DatabaseSchemaUpdaterTest, SequenceNotValidMissingDownFile) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->WriteMigrationFile(migration_file_up_2_valid);
    this->WriteMigrationFile(migration_file_up_3_valid);
    this->WriteMigrationFile(migration_file_up_4_valid);
    this->WriteMigrationFile(migration_file_down_3_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 1));
    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 2));
    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 3));

    this->ExpectUserVersion(0);
    EXPECT_FALSE(this->DoesTableExist(table1)); // Database was not changed
}

TEST_F(DatabaseSchemaUpdaterTest, SequenceNotValidMissingUpFile) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->WriteMigrationFile(migration_file_up_2_valid);
    this->WriteMigrationFile(migration_file_down_2_valid);
    this->WriteMigrationFile(migration_file_down_3_valid);
    this->WriteMigrationFile(migration_file_down_4_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 1));
    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 2));
    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 3));

    this->ExpectUserVersion(0);
    EXPECT_FALSE(this->DoesTableExist(table1)); // Database was not changed
}

TEST_F(DatabaseSchemaUpdaterTest, ApplyMultipleMigrationFilesStepByStep) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->WriteMigrationFile(migration_file_up_2_valid);
    this->WriteMigrationFile(migration_file_up_3_valid);
    this->WriteMigrationFile(migration_file_down_2_valid);
    this->WriteMigrationFile(migration_file_down_3_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);
    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_FALSE(this->DoesTableExist(table2));
    EXPECT_FALSE(this->DoesTableExist(table3));

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 2));
    this->ExpectUserVersion(2);
    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_TRUE(this->DoesTableExist(table2));
    EXPECT_FALSE(this->DoesTableExist(table3));

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 3));
    this->ExpectUserVersion(3);
    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_TRUE(this->DoesTableExist(table2));
    EXPECT_TRUE(this->DoesTableExist(table3));

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 2));
    this->ExpectUserVersion(2);
    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_TRUE(this->DoesTableExist(table2));
    EXPECT_FALSE(this->DoesTableExist(table3));

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);
    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_FALSE(this->DoesTableExist(table2));
    EXPECT_FALSE(this->DoesTableExist(table3));
}

TEST_F(DatabaseSchemaUpdaterTest, ApplyMultipleMigrationFilesAtOnce) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->WriteMigrationFile(migration_file_up_2_valid);
    this->WriteMigrationFile(migration_file_up_3_valid);
    this->WriteMigrationFile(migration_file_down_2_valid);
    this->WriteMigrationFile(migration_file_down_3_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 3));
    this->ExpectUserVersion(3);
    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_TRUE(this->DoesTableExist(table2));
    EXPECT_TRUE(this->DoesTableExist(table3));

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);
    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_FALSE(this->DoesTableExist(table2));
    EXPECT_FALSE(this->DoesTableExist(table3));
}

TEST_F(DatabaseSchemaUpdaterTest, ApplyMultipleMigrationFilesAtOnceWithFailure) {

    this->WriteMigrationFile(migration_file_up_1_valid);
    this->WriteMigrationFile(migration_file_up_2_valid);
    this->WriteMigrationFile(migration_file_up_3_valid);
    this->WriteMigrationFile(migration_file_down_2_valid);
    this->WriteMigrationFile(migration_file_down_3_valid);

    SchemaUpdater updater{this->database.get()};

    EXPECT_TRUE(updater.apply_migration_files(this->migration_files_path, 1));
    this->ExpectUserVersion(1);
    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_FALSE(this->DoesTableExist(table2));

    EXPECT_TRUE(this->database->execute_statement(migration_file_up_2_valid.content.data()));

    EXPECT_TRUE(this->DoesTableExist(table2));

    EXPECT_FALSE(updater.apply_migration_files(this->migration_files_path, 3));

    EXPECT_TRUE(this->DoesTableExist(table1));
    EXPECT_TRUE(this->DoesTableExist(table2));
    EXPECT_FALSE(this->DoesTableExist(table3));

    this->ExpectUserVersion(1);
}

} // namespace everest::db::sqlite
