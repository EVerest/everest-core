// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ocpp/common/database/database_connection.hpp>
#include <ocpp/common/types.hpp>

namespace ocpp::common {

/// \brief Base class for database-related exceptions
class DatabaseException : public std::exception {
public:
    explicit DatabaseException(const std::string& message) : msg(message) {
    }
    virtual ~DatabaseException() noexcept {
    }

    virtual const char* what() const noexcept {
        return msg.c_str();
    }

protected:
    std::string msg;
};

/// \brief Exception for database connection errors
class DatabaseConnectionException : public DatabaseException {
public:
    explicit DatabaseConnectionException(const std::string& message) : DatabaseException(message) {
    }
};

/// \brief Exception that is used if expected table entries are not found
class RequiredEntryNotFoundException : public DatabaseException {
public:
    explicit RequiredEntryNotFoundException(const std::string& message) : DatabaseException(message) {
    }
};

/// \brief Exception for errors during database migration
class DatabaseMigrationException : public DatabaseException {
public:
    explicit DatabaseMigrationException(const std::string& message) : DatabaseException(message) {
    }
};

struct DBTransactionMessage {
    json json_message;
    std::string message_type;
    int32_t message_attempts;
    DateTime timestamp;
    std::string unique_id;
};

class DatabaseHandlerCommon {
protected:
    std::unique_ptr<DatabaseConnectionInterface> database;
    const fs::path sql_migration_files_path;
    const uint32_t target_schema_version;

    /// \brief Perform the initialization needed to use the database. Will be called by open_connection()
    virtual void init_sql() = 0;

public:
    /// \brief Common database handler class
    /// Class handles some common database functionality like inserting and removing transaction messages.
    ///
    /// \param database Interface for the database connection
    /// \param sql_migration_files_path Filesystem path to migration file folder
    /// \param target_schema_version The required schema version of the database
    explicit DatabaseHandlerCommon(std::unique_ptr<DatabaseConnectionInterface> database,
                                   const fs::path& sql_migration_files_path, uint32_t target_schema_version) noexcept;

    ~DatabaseHandlerCommon() = default;

    /// \brief Opens connection to database file and performs the initialization by calling init_sql()
    void open_connection();

    /// \brief Closes the database connection.
    void close_connection();

    /// \brief Get transaction messages from transaction messages queue table.
    /// \return The transaction messages.
    virtual std::vector<DBTransactionMessage> get_transaction_messages();

    /// \brief Insert a new transaction message that needs to be sent to the CSMS.
    /// \param transaction_message  The message to be stored.
    virtual void insert_transaction_message(const DBTransactionMessage& transaction_message);

    /// \brief Remove a transaction message from the database.
    /// \param unique_id    The unique id of the transaction message.
    /// \return True on success.
    virtual void remove_transaction_message(const std::string& unique_id);

    /// \brief Deletes all entries from TRANSACTION_QUEUE table
    virtual void clear_transaction_queue();
};

} // namespace ocpp::common
