// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/database/exceptions.hpp>
#include <everest/database/sqlite/connection.hpp>
#include <everest/database/sqlite/statement.hpp>
#include <gtest/gtest.h>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace everest::db::sqlite {

class SQLiteStatementTest : public ::testing::Test {
protected:
    std::unique_ptr<Connection> db;

    void SetUp() override {
        fs::path db_path = "file::memory:?cache=shared";
        db = std::make_unique<Connection>(db_path);
        ASSERT_TRUE(db->open_connection());

        ASSERT_TRUE(db->execute_statement(
            "CREATE TABLE test_table (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, value INTEGER, score REAL);"));
    }

    void TearDown() override {
        db->close_connection();
    }
};

TEST_F(SQLiteStatementTest, InsertAndQueryRow) {
    auto insert_stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES (:name, :value, :score);");

    insert_stmt->bind_text(":name", "test_name", SQLiteString::Transient);
    insert_stmt->bind_int(":value", 42);
    insert_stmt->bind_double(":score", 98.6);

    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT name, value, score FROM test_table WHERE id = 1;");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);

    EXPECT_EQ(select_stmt->column_text(0), "test_name");
    EXPECT_EQ(select_stmt->column_int(1), 42);
    EXPECT_DOUBLE_EQ(select_stmt->column_double(2), 98.6);
}

TEST_F(SQLiteStatementTest, NullBindingAndOptionalResult) {
    auto insert_stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES (?, ?, ?);");
    insert_stmt->bind_null(1);
    insert_stmt->bind_int(2, 100);
    insert_stmt->bind_null(3);
    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT name, score FROM test_table WHERE id = 1;");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);

    EXPECT_FALSE(select_stmt->column_text_nullable(0).has_value());
    EXPECT_FALSE(select_stmt->column_text_nullable(1).has_value());
}

TEST_F(SQLiteStatementTest, InvalidParameterThrows) {
    auto stmt = db->new_statement("SELECT * FROM test_table WHERE name = :name;");
    EXPECT_THROW(stmt->bind_int(":invalid", 1), std::out_of_range);
}

TEST_F(SQLiteStatementTest, ResetAndReuseStatement) {
    auto insert_stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES (?, ?, ?);");

    insert_stmt->bind_text(1, "row1");
    insert_stmt->bind_int(2, 1);
    insert_stmt->bind_double(3, 1.1);
    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    ASSERT_EQ(insert_stmt->reset(), SQLITE_OK);

    insert_stmt->bind_text(1, "row2");
    insert_stmt->bind_int(2, 2);
    insert_stmt->bind_double(3, 2.2);
    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT COUNT(*) FROM test_table;");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);
    EXPECT_EQ(select_stmt->column_int(0), 2);
}

TEST_F(SQLiteStatementTest, BindByNameAndIndexConsistency) {
    auto stmt_by_index = db->new_statement("INSERT INTO test_table (name, value, score) VALUES (?, ?, ?);");
    auto stmt_by_name =
        db->new_statement("INSERT INTO test_table (name, value, score) VALUES (:name, :value, :score);");

    stmt_by_index->bind_text(1, "index_row");
    stmt_by_index->bind_int(2, 123);
    stmt_by_index->bind_double(3, 1.23);
    ASSERT_EQ(stmt_by_index->step(), SQLITE_DONE);

    stmt_by_name->bind_text(":name", "name_row");
    stmt_by_name->bind_int(":value", 321);
    stmt_by_name->bind_double(":score", 3.21);
    ASSERT_EQ(stmt_by_name->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT COUNT(*) FROM test_table;");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);
    EXPECT_EQ(select_stmt->column_int(0), 2);
}

TEST_F(SQLiteStatementTest, BindInt64AndReadBack) {
    int64_t large_value = 9223372036854775807LL; // max int64

    auto insert_stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES (?, ?, ?);");
    insert_stmt->bind_text(1, "int64_test");
    insert_stmt->bind_int64(2, large_value);
    insert_stmt->bind_double(3, 0.0);
    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT value FROM test_table WHERE name = 'int64_test';");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);
    EXPECT_EQ(select_stmt->column_int64(0), large_value);
}

TEST_F(SQLiteStatementTest, BindInt64NamedParameter) {
    int64_t big_value = 1234567890123456789LL;

    auto insert_stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES (:name, :value, :score);");

    insert_stmt->bind_text(":name", "int64_named", SQLiteString::Transient);
    insert_stmt->bind_int64(":value", big_value);
    insert_stmt->bind_double(":score", 42.42);

    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT value FROM test_table WHERE name = 'int64_named';");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);
    EXPECT_EQ(select_stmt->column_int64(0), big_value);
}

TEST_F(SQLiteStatementTest, StatementDestructorFinalizesStatement) {
    {
        auto stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES ('temp', 0, 0.0);");
        ASSERT_EQ(stmt->step(), SQLITE_DONE);
        // stmt goes out of scope here
    }

    auto select_stmt = db->new_statement("SELECT COUNT(*) FROM test_table WHERE name = 'temp';");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);
    EXPECT_EQ(select_stmt->column_int(0), 1);
}

TEST_F(SQLiteStatementTest, GetNumberOfColumnsInRow) {
    auto insert_stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES ('check_cols', 1, 1.0);");
    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT id, name, value, score FROM test_table WHERE name = 'check_cols';");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);
    EXPECT_EQ(select_stmt->get_number_of_rows(), 4);
}

TEST_F(SQLiteStatementTest, ColumnTypeChecks) {
    auto insert_stmt =
        db->new_statement("INSERT INTO test_table (name, value, score) VALUES ('type_test', 999, 9.99);");
    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT name, value, score FROM test_table WHERE name = 'type_test';");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);

    EXPECT_EQ(select_stmt->column_type(0), SQLITE_TEXT);
    EXPECT_EQ(select_stmt->column_type(1), SQLITE_INTEGER);
    EXPECT_EQ(select_stmt->column_type(2), SQLITE_FLOAT);
}

TEST_F(SQLiteStatementTest, BindNullByName) {
    auto insert_stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES (:name, :value, :score);");
    insert_stmt->bind_null(":name");
    insert_stmt->bind_int(":value", 5);
    insert_stmt->bind_null(":score");
    ASSERT_EQ(insert_stmt->step(), SQLITE_DONE);

    auto select_stmt = db->new_statement("SELECT name, score FROM test_table WHERE value = 5;");
    ASSERT_EQ(select_stmt->step(), SQLITE_ROW);

    EXPECT_FALSE(select_stmt->column_text_nullable(0).has_value());
    EXPECT_FALSE(select_stmt->column_text_nullable(1).has_value());
}

TEST_F(SQLiteStatementTest, ResetPreservesPreparedStatement) {
    auto stmt = db->new_statement("INSERT INTO test_table (name, value, score) VALUES ('x', 1, 1.0);");
    ASSERT_EQ(stmt->step(), SQLITE_DONE);
    ASSERT_EQ(stmt->reset(), SQLITE_OK);

    stmt->bind_text(1, "x"); // optional rebind test
    stmt->bind_int(2, 2);
    stmt->bind_double(3, 2.0);
    ASSERT_EQ(stmt->step(), SQLITE_DONE);
}

} // namespace everest::db::sqlite
