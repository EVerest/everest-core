// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest/database/sqlite/connection.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std::string_literals;

namespace everest::db::sqlite {

class DatabaseTestingUtils : public ::testing::Test {

protected:
    std::unique_ptr<ConnectionInterface> database;

public:
    DatabaseTestingUtils() : database(std::make_unique<Connection>("file::memory:?cache=shared")) {
        EXPECT_TRUE(this->database->open_connection());
    }

    void ExpectUserVersion(uint32_t expected_version) {
        auto statement = this->database->new_statement("PRAGMA user_version");

        EXPECT_EQ(statement->step(), SQLITE_ROW);
        EXPECT_EQ(statement->column_int(0), expected_version);
    }

    void SetUserVersion(uint32_t user_version) {
        EXPECT_TRUE(this->database->execute_statement("PRAGMA user_version = "s + std::to_string(user_version)));
    }

    bool DoesTableExist(std::string_view table) {
        const std::string statement = "SELECT name FROM sqlite_master WHERE type='table' AND name=@table_name";

        std::unique_ptr<StatementInterface> table_exists_statement = this->database->new_statement(statement);
        table_exists_statement->bind_text("@table_name", std::string(table), SQLiteString::Transient);
        const int status = table_exists_statement->step();
        const int number_of_rows = table_exists_statement->get_number_of_rows();
        return status != SQLITE_ERROR && number_of_rows == 1;
    }

    bool DoesColumnExist(std::string_view table, std::string_view column) {
        return this->database->execute_statement("SELECT "s + column.data() + " FROM " + table.data() + " LIMIT 1;");
    }
};

} // namespace everest::db::sqlite
