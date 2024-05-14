// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/database/database_handler_common.hpp>

#include <everest/logging.hpp>
#include <ocpp/common/database/database_schema_updater.hpp>

namespace ocpp::common {

DatabaseHandlerCommon::DatabaseHandlerCommon(std::unique_ptr<DatabaseConnectionInterface> database,
                                             const fs::path& sql_migration_files_path,
                                             uint32_t target_schema_version) noexcept :
    database(std::move(database)),
    sql_migration_files_path(sql_migration_files_path),
    target_schema_version(target_schema_version) {
}

void DatabaseHandlerCommon::open_connection() {
    DatabaseSchemaUpdater updater{this->database.get()};

    if (!updater.apply_migration_files(this->sql_migration_files_path, target_schema_version)) {
        throw DatabaseMigrationException("SQL migration failed");
    }

    if (!this->database->open_connection()) {
        throw DatabaseConnectionException("Could not open database at provided path.");
    }

    this->init_sql();
}

void DatabaseHandlerCommon::close_connection() {
    this->database->close_connection();
}

std::vector<DBTransactionMessage> DatabaseHandlerCommon::get_transaction_messages() {
    std::vector<DBTransactionMessage> transaction_messages;

    std::string sql =
        "SELECT UNIQUE_ID, MESSAGE, MESSAGE_TYPE, MESSAGE_ATTEMPTS, MESSAGE_TIMESTAMP FROM TRANSACTION_QUEUE";

    auto stmt = this->database->new_statement(sql);

    int status;
    while ((status = stmt->step()) == SQLITE_ROW) {
        try {
            const std::string message = stmt->column_text(1);
            const std::string unique_id = stmt->column_text(0);
            const std::string message_type = stmt->column_text(2);
            const std::string message_timestamp = stmt->column_text(4);
            const int message_attempts = stmt->column_int(3);

            json json_message = json::parse(message);

            DBTransactionMessage control_message;
            control_message.message_attempts = message_attempts;
            control_message.timestamp = message_timestamp;
            control_message.message_type = message_type;
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
        throw QueryExecutionException(this->database->get_error_message());
    }

    return transaction_messages;
}

void DatabaseHandlerCommon::insert_transaction_message(const DBTransactionMessage& transaction_message) {
    const std::string sql =
        "INSERT INTO TRANSACTION_QUEUE (UNIQUE_ID, MESSAGE, MESSAGE_TYPE, MESSAGE_ATTEMPTS, MESSAGE_TIMESTAMP) VALUES "
        "(@unique_id, @message, @message_type, @message_attempts, @message_timestamp)";

    auto stmt = this->database->new_statement(sql);

    const std::string message = transaction_message.json_message.dump();
    stmt->bind_text("@unique_id", transaction_message.unique_id);
    stmt->bind_text("@message", message);
    stmt->bind_text("@message_type", transaction_message.message_type);
    stmt->bind_int("@message_attempts", transaction_message.message_attempts);
    stmt->bind_text("@message_timestamp", transaction_message.timestamp.to_rfc3339(), SQLiteString::Transient);

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandlerCommon::remove_transaction_message(const std::string& unique_id) {
    std::string sql = "DELETE FROM TRANSACTION_QUEUE WHERE UNIQUE_ID = @unique_id";

    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@unique_id", unique_id);

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandlerCommon::clear_transaction_queue() {
    const auto retval = this->database->clear_table("TRANSACTION_QUEUE");
    if (retval == false) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

} // namespace ocpp::common
