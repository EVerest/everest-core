// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <ocpp/common/database_handler_base.hpp>

namespace ocpp::common {

DatabaseHandlerBase::DatabaseHandlerBase() noexcept : db(nullptr) {
}

std::vector<DBTransactionMessage> DatabaseHandlerBase::get_transaction_messages() {
    std::vector<DBTransactionMessage> transaction_messages;

    try {
        std::string sql =
            "SELECT UNIQUE_ID, MESSAGE, MESSAGE_TYPE, MESSAGE_ATTEMPTS, MESSAGE_TIMESTAMP FROM TRANSACTION_QUEUE";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(this->db, sql.c_str(), static_cast<int>(sql.size()), &stmt, nullptr) != SQLITE_OK) {
            EVLOG_error << "Could not prepare insert statement: " << sqlite3_errmsg(this->db);
            throw std::runtime_error("Could not get transaction queue");
        }

        int status;
        while ((status = sqlite3_step(stmt)) == SQLITE_ROW) {
            try {
                const std::string message{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))};
                const std::string unique_id{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))};
                const std::string message_type{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))};
                const std::string message_timestamp{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))};
                const int message_attempts = sqlite3_column_int(stmt, 3);

                json json_message = json::parse(message);

                DBTransactionMessage control_message;
                control_message.message_attempts = message_attempts;
                control_message.timestamp = message_timestamp;
                control_message.message_type = /*conversions::string_to_messagetype(*/ message_type;
                control_message.unique_id = unique_id;
                control_message.json_message = json_message;
                transaction_messages.push_back(std::move(control_message));
            } catch (const json::exception& e) {
                EVLOG_error << "json parse failed because: "
                            << "(" << e.what() << ")";
            } catch (const std::exception& e) {
                EVLOG_error << "can not get queued transaction message from database: "
                            << "(" << e.what() << ")";
            }
        }

        if (status != SQLITE_DONE) {
            EVLOG_error << "Could not get (all) queued transaction messages from database";
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Could not get queued transaction messages from database: ") + e.what());
    }

    return transaction_messages;
}

bool DatabaseHandlerBase::insert_transaction_message(const DBTransactionMessage& transaction_message) {
    const std::string sql =
        "INSERT INTO TRANSACTION_QUEUE (UNIQUE_ID, MESSAGE, MESSAGE_TYPE, MESSAGE_ATTEMPTS, MESSAGE_TIMESTAMP) VALUES "
        "(@unique_id, @message, @message_type, @message_attempts, @message_timestamp)";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), static_cast<int>(sql.size()), &stmt, nullptr) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement: " << sqlite3_errmsg(this->db);
        throw std::runtime_error("Error while inserting queued transaction message into database");
    }

    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@unique_id"), transaction_message.unique_id.c_str(),
                      static_cast<int>(transaction_message.unique_id.length()), SQLITE_TRANSIENT);

    const std::string message = transaction_message.json_message.dump();
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@message"), message.c_str(),
                      static_cast<int>(message.size()), SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@message_type"),
                      transaction_message.message_type.c_str(),
                      static_cast<int>(transaction_message.message_type.size()), SQLITE_TRANSIENT);

    sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@message_attempts"),
                     transaction_message.message_attempts);

    const std::string timestamp = transaction_message.timestamp.to_rfc3339();
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@message_timestamp"), timestamp.c_str(),
                      static_cast<int>(timestamp.size()), SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into TRANSACTION_QUEUE table: " << sqlite3_errmsg(db);
        return false;
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into TRANSACTION_QUEUE table: " << sqlite3_errmsg(this->db);
        return false;
    }

    return true;
}

void DatabaseHandlerBase::remove_transaction_message(const std::string& unique_id) {
    try {
        std::string sql = "DELETE FROM TRANSACTION_QUEUE WHERE UNIQUE_ID = @unique_id";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(this->db, sql.c_str(), static_cast<int>(sql.size()), &stmt, nullptr) != SQLITE_OK) {
            EVLOG_error << "Could not prepare insert statement: " << sqlite3_errmsg(this->db);
            return;
        }

        sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@unique_id"), unique_id.c_str(),
                          static_cast<int>(unique_id.size()), nullptr);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            EVLOG_error << "Could not delete from table: " << sqlite3_errmsg(this->db);
        }
        if (sqlite3_finalize(stmt) != SQLITE_OK) {
            EVLOG_error << "Error deleting from table: " << sqlite3_errmsg(this->db);
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Exception while deleting from transaction queue table: " << e.what();
    }
}

} // namespace ocpp::common
