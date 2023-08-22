// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <deque>
#include <memory>
#include <sqlite3.h>
#include <string>

#include <ocpp/common/types.hpp>

namespace ocpp::common {

struct DBTransactionMessage {
    json json_message;
    std::string message_type;
    int32_t message_attempts;
    DateTime timestamp;
    std::string unique_id;
};

class DatabaseHandlerBase {
protected:
    sqlite3* db;

public:
    ///
    /// \brief Base database handler class.
    ///
    /// Other database classes should derive from this class and only use the functions after `db` is initialized.
    /// Class handles some common database functionality like inserting and removing transaction messages.
    ///
    /// \warning The 'db' variable is not initialized, the deriving class should do that.
    ///
    DatabaseHandlerBase() noexcept;

    /// \brief Get transaction messages from transaction messages queue table.
    /// \return The transaction messages.
    std::vector<DBTransactionMessage> get_transaction_messages();

    /// \brief Insert a new transaction message that needs to be sent to the CSMS.
    /// \param transaction_message  The message to be stored.
    /// \return True on success.
    bool insert_transaction_message(const DBTransactionMessage& transaction_message);

    /// \brief Remove a transaction message from the database.
    /// \param unique_id    The unique id of the transaction message.
    /// \return True on success.
    void remove_transaction_message(const std::string& unique_id);
};

} // namespace ocpp::common
