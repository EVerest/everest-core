// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ocpp/common/database/database_connection.hpp>
#include <ocpp/common/database/database_exceptions.hpp>
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

    /// \brief Get messages from messages queue table specified by \p queue_type
    /// \param queue_type , defaults to QueueType::Transaction
    /// \return The transaction messages.
    virtual std::vector<DBTransactionMessage>
    get_message_queue_messages(const QueueType queue_type = QueueType::Transaction);

    /// \brief Insert a new message into messages queue table specified by \p queue_type
    /// \param message  The message to be stored.
    /// \param queue_type , defaults to QueueType::Transaction
    virtual void insert_message_queue_message(const DBTransactionMessage& message,
                                              const QueueType queue_type = QueueType::Transaction);

    /// \brief Remove a message from the messages queue table specified by \p queue_type
    /// \param unique_id    The unique id of the transaction message
    /// \param queue_type , defaults to QueueType::Transaction
    /// \return True on success.
    virtual void remove_message_queue_message(const std::string& unique_id,
                                              const QueueType queue_type = QueueType::Transaction);

    /// \brief Deletes all entries from message queue table specified by \p queue_type
    /// \param queue_type , defaults to QueueType::Transaction
    virtual void clear_message_queue(const QueueType queue_type = QueueType::Transaction);
};

} // namespace ocpp::common
