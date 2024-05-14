// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/database/database_connection.hpp>

#include <everest/logging.hpp>

using namespace std::chrono_literals;
using namespace std::string_literals;

namespace ocpp::common {

class DatabaseTransaction : public DatabaseTransactionInterface {
private:
    DatabaseConnection& database;
    std::unique_lock<std::timed_mutex> mutex;

public:
    DatabaseTransaction(DatabaseConnection& database, std::unique_lock<std::timed_mutex> mutex) :
        database{database}, mutex{std::move(mutex)} {
        this->database.execute_statement("BEGIN TRANSACTION");
    }

    // Will by default rollback the transaction if destructed
    ~DatabaseTransaction() override {
        if (this->mutex.owns_lock()) {
            this->rollback();
        }
    }

    void commit() override {
        const auto retval = this->database.execute_statement("COMMIT TRANSACTION");
        this->mutex.unlock();
        if (retval == false) {
            throw QueryExecutionException(this->database.get_error_message());
        }
    }
    void rollback() override {
        const auto retval = this->database.execute_statement("ROLLBACK TRANSACTION");
        this->mutex.unlock();
        if (retval == false) {
            throw QueryExecutionException(this->database.get_error_message());
        }
    }
};

DatabaseConnection::DatabaseConnection(const fs::path& database_file_path) noexcept :
    db(nullptr), database_file_path(database_file_path), open_count(0) {
}

DatabaseConnection::~DatabaseConnection() {
    // There could still be a transaction active and we have no way to abort it, so wait a few seconds to give it time
    // to finish
    auto lock = std::unique_lock(this->transaction_mutex, 2s);
    close_connection_internal(true);
}

bool DatabaseConnection::open_connection() {
    if (this->open_count.fetch_add(1) != 0) {
        EVLOG_debug << "Connection already opened";
        return true;
    }

    // Add special exception for databases in ram; we don't need to create a path for them
    if (this->database_file_path.string().find(":memory:") == std::string::npos and
        !fs::exists(this->database_file_path.parent_path())) {
        fs::create_directories(this->database_file_path.parent_path());
    }

    if (sqlite3_open(this->database_file_path.c_str(), &this->db) != SQLITE_OK) {
        EVLOG_error << "Error opening database at " << this->database_file_path << ": " << sqlite3_errmsg(db);
        return false;
    }
    EVLOG_info << "Established connection to database: " << this->database_file_path;
    return true;
}

bool DatabaseConnection::close_connection() {
    return this->close_connection_internal(false);
}

bool DatabaseConnection::close_connection_internal(bool force_close) {
    if (!force_close && this->open_count.fetch_sub(1) != 1) {
        EVLOG_debug << "Connection should remain open for other users";
        return true;
    }

    if (this->db == nullptr) {
        EVLOG_info << "Database file " << this->database_file_path << " is already closed";
        return true;
    }

    // forcefully finalize all statements before calling sqlite3_close
    sqlite3_stmt* stmt = nullptr;
    while ((stmt = sqlite3_next_stmt(db, stmt)) != nullptr) {
        sqlite3_finalize(stmt);
    }

    if (sqlite3_close_v2(this->db) != SQLITE_OK) {
        EVLOG_error << "Error closing database file " << this->database_file_path << ": " << this->get_error_message();
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

std::unique_ptr<DatabaseTransactionInterface> DatabaseConnection::begin_transaction() {
    return std::make_unique<DatabaseTransaction>(*this, std::unique_lock(this->transaction_mutex));
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
