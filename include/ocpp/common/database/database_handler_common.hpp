// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ocpp/common/database/database_connection.hpp>
#include <ocpp/common/types.hpp>

namespace ocpp::common {

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
    /// \return True on success.
    virtual bool insert_transaction_message(const DBTransactionMessage& transaction_message);

    /// \brief Remove a transaction message from the database.
    /// \param unique_id    The unique id of the transaction message.
    /// \return True on success.
    virtual void remove_transaction_message(const std::string& unique_id);
};

} // namespace ocpp::common
