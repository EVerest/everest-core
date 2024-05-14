// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <mutex>
#include <sqlite3.h>

#include <ocpp/common/support_older_cpp_versions.hpp>

#include "sqlite_statement.hpp"

namespace ocpp::common {

/// \brief Helper class for transactions. Will lock the database interface from new transaction until commit() or
/// rollback() is called or the object destroyed
class DatabaseTransactionInterface {
public:
    /// \brief Destructor of transaction: Will by default rollback unless commit() is called
    virtual ~DatabaseTransactionInterface() = default;

    /// \brief Commits the transaction and release the lock on the database interface
    virtual void commit() = 0;

    /// \brief Aborts the transaction and release the lock on the database interface
    virtual void rollback() = 0;
};

class DatabaseConnectionInterface {
public:
    virtual ~DatabaseConnectionInterface() = default;

    /// \brief Opens the database connection. Returns true if succeeded.
    virtual bool open_connection() = 0;

    /// \brief Closes the database connection. Returns true if succeeded.
    virtual bool close_connection() = 0;

    /// \brief Start a transaction on the database. Returns an object holding the transaction.
    /// \note This function can block until the previous transaction is finished.
    [[nodiscard]] virtual std::unique_ptr<DatabaseTransactionInterface> begin_transaction() = 0;

    /// \brief Immediately executes \p statement. Returns true if succeeded.
    virtual bool execute_statement(const std::string& statement) = 0;

    /// \brief Returns a new SQLiteStatementInterface to be used to perform more advanced sql statements.
    /// \note Will throw an std::runtime_error if the statement can't be prepared
    virtual std::unique_ptr<SQLiteStatementInterface> new_statement(const std::string& sql) = 0;

    /// \brief Returns the latest error message from sqlite3.
    virtual const char* get_error_message() = 0;

    /// \brief Clears the table with name \p table. Returns true if succeeded.
    virtual bool clear_table(const std::string& table) = 0;

    /// \brief Gets the last inserted rowid.
    virtual int64_t get_last_inserted_rowid() = 0;
};

class DatabaseConnection : public DatabaseConnectionInterface {
private:
    sqlite3* db;
    const fs::path database_file_path;
    std::atomic_uint32_t open_count;
    std::timed_mutex transaction_mutex;

    bool close_connection_internal(bool force_close);

public:
    explicit DatabaseConnection(const fs::path& database_file_path) noexcept;

    virtual ~DatabaseConnection();

    bool open_connection() override;
    bool close_connection() override;

    [[nodiscard]] std::unique_ptr<DatabaseTransactionInterface> begin_transaction() override;

    bool execute_statement(const std::string& statement) override;
    std::unique_ptr<SQLiteStatementInterface> new_statement(const std::string& sql) override;

    const char* get_error_message() override;

    bool clear_table(const std::string& table) override;

    int64_t get_last_inserted_rowid() override;
};

} // namespace ocpp::common
