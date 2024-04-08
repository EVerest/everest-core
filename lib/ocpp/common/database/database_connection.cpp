// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/database/database_connection.hpp>

#include <everest/logging.hpp>

using namespace std::string_literals;

namespace ocpp::common {

DatabaseConnection::DatabaseConnection(const fs::path& database_file_path) noexcept :
    db(nullptr), database_file_path(database_file_path) {
}

DatabaseConnection::~DatabaseConnection() {
    close_connection();
}

bool DatabaseConnection::open_connection() {
    // Add special exception for databases in ram; we don't need to create a path for them
    if (this->database_file_path.string().find(":memory:") == std::string::npos and
        !fs::exists(this->database_file_path.parent_path())) {
        fs::create_directories(this->database_file_path.parent_path());
    }

    if (sqlite3_open(this->database_file_path.c_str(), &this->db) != SQLITE_OK) {
        EVLOG_error << "Error opening database at " << this->database_file_path << ": " << sqlite3_errmsg(db);
        return false;
    }
    EVLOG_info << "Established connection to Database: " << this->database_file_path;
    return true;
}

bool DatabaseConnection::close_connection() {
    EVLOG_info << "Closing database connection...";
    if (this->db == nullptr) {
        EVLOG_info << "Already closed";
        return true;
    }

    // forcefully finalize all statements before calling sqlite3_close
    sqlite3_stmt* stmt = nullptr;
    while ((stmt = sqlite3_next_stmt(db, stmt)) != nullptr) {
        sqlite3_finalize(stmt);
    }

    if (sqlite3_close_v2(this->db) != SQLITE_OK) {
        EVLOG_error << "Error closing database file: " << this->get_error_message();
        return false;
    }
    EVLOG_info << "Successfully closed database: " << this->database_file_path;
    this->db = nullptr;
    return true;
}

bool DatabaseConnection::execute_statement(const std::string& statement) {
    char* err_msg = nullptr;
    if (sqlite3_exec(this->db, statement.c_str(), NULL, NULL, &err_msg) != SQLITE_OK) {
        EVLOG_error << "Could not execute statement \"" << statement << "\": " << err_msg;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

const char* DatabaseConnection::get_error_message() {
    return sqlite3_errmsg(this->db);
}

bool DatabaseConnection::begin_transaction() {
    return this->execute_statement("BEGIN TRANSACTION");
}

bool DatabaseConnection::commit_transaction() {
    return this->execute_statement("COMMIT TRANSACTION");
}

bool DatabaseConnection::rollback_transaction() {
    return this->execute_statement("ROLLBACK TRANSACTION");
}

std::unique_ptr<SQLiteStatementInterface> DatabaseConnection::new_statement(const std::string& sql) {
    return std::make_unique<SQLiteStatement>(this->db, sql);
}

bool DatabaseConnection::clear_table(const std::string& table) {
    return this->execute_statement("DELETE FROM "s + table);
}

int64_t DatabaseConnection::get_last_inserted_rowid() {
    return sqlite3_last_insert_rowid(this->db);
}

} // namespace ocpp::common