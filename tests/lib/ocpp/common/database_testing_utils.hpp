// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ocpp/common/database/database_connection.hpp>

using namespace ocpp;
using namespace ocpp::common;
using namespace std::string_literals;

class DatabaseTestingUtils : public ::testing::Test {

protected:
    std::unique_ptr<DatabaseConnectionInterface> database;

public:
    DatabaseTestingUtils() : database(std::make_unique<DatabaseConnection>("file::memory:?cache=shared")) {
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
        return this->database->clear_table(table.data());
    }

    bool DoesColumnExist(std::string_view table, std::string_view column) {
        return this->database->execute_statement("SELECT "s + column.data() + " FROM " + table.data() + " LIMIT 1;");
    }
};