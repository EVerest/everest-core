// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/message_queue.hpp>
#include <ocpp/common/sqlite_statement.hpp>
#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/types.hpp>

namespace fs = std::filesystem;

namespace ocpp {
namespace v201 {

DatabaseHandler::DatabaseHandler(const fs::path& database_path, const fs::path& sql_init_path) :
    ocpp::common::DatabaseHandlerBase(), sql_init_path(sql_init_path) {
    if (!fs::exists(database_path)) {
        fs::create_directories(database_path);
    }
    this->database_file_path = database_path / "cp.db";
};

DatabaseHandler::~DatabaseHandler() {
    if (sqlite3_close_v2(this->db) == SQLITE_OK) {
        EVLOG_debug << "Successfully closed database file";
    } else {
        EVLOG_error << "Error closing database file: " << sqlite3_errmsg(this->db);
    }
}

void DatabaseHandler::sql_init() {
    EVLOG_debug << "Running SQL initialization script.";
    std::ifstream t(this->sql_init_path.string());
    std::stringstream init_sql;

    init_sql << t.rdbuf();

    char* err = NULL;

    if (sqlite3_exec(this->db, init_sql.str().c_str(), NULL, NULL, &err) != SQLITE_OK) {
        EVLOG_error << "Could not create tables: " << sqlite3_errmsg(this->db);
        throw std::runtime_error("Database access error");
    }
}

bool DatabaseHandler::clear_table(const std::string& table_name) {
    char* err_msg = 0;
    std::string sql = "DELETE FROM " + table_name + ";";
    if (sqlite3_exec(this->db, sql.c_str(), NULL, NULL, &err_msg) != SQLITE_OK) {
        return false;
    }
    return true;
}

void DatabaseHandler::open_connection() {
    if (sqlite3_open(this->database_file_path.c_str(), &this->db) != SQLITE_OK) {
        EVLOG_error << "Error opening database at " << this->database_file_path.c_str() << ": " << sqlite3_errmsg(db);
        throw std::runtime_error("Could not open database at provided path.");
    }
    EVLOG_debug << "Established connection to Database.";
    this->sql_init();
}

void DatabaseHandler::close_connection() {
    if (sqlite3_close(this->db) == SQLITE_OK) {
        EVLOG_debug << "Successfully closed database file";
    } else {
        EVLOG_error << "Error closing database file: " << sqlite3_errmsg(this->db);
    }
}

void DatabaseHandler::insert_auth_cache_entry(const std::string& id_token_hash, const IdTokenInfo& id_token_info) {
    std::string sql = "INSERT OR REPLACE INTO AUTH_CACHE (ID_TOKEN_HASH, ID_TOKEN_INFO) VALUES "
                      "(@id_token_hash, @id_token_info)";
    SQLiteStatement insert_stmt(this->db, sql);

    insert_stmt.bind_text("@id_token_hash", id_token_hash);
    insert_stmt.bind_text("@id_token_info", json(id_token_info).dump(), SQLiteString::Transient);

    if (insert_stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into AUTH_CACHE table: " << sqlite3_errmsg(db);
        return;
    }
}

std::optional<IdTokenInfo> DatabaseHandler::get_auth_cache_entry(const std::string& id_token_hash) {
    try {
        std::string sql = "SELECT ID_TOKEN_INFO FROM AUTH_CACHE WHERE ID_TOKEN_HASH = @id_token_hash";
        SQLiteStatement select_stmt(this->db, sql);

        select_stmt.bind_text("@id_token_hash", id_token_hash);

        if (select_stmt.step() != SQLITE_ROW) {
            return std::nullopt;
        }
        return IdTokenInfo(json::parse(select_stmt.column_text(0)));
    } catch (const json::exception& e) {
        EVLOG_warning << "Could not parse data of IdTokenInfo: " << e.what();
        return std::nullopt;
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown Error while parsing IdTokenInfo: " << e.what();
        return std::nullopt;
    }
}

void DatabaseHandler::delete_auth_cache_entry(const std::string& id_token_hash) {
    try {
        std::string sql = "DELETE FROM AUTH_CACHE WHERE ID_TOKEN_HASH = @id_token_hash";
        SQLiteStatement delete_stmt(this->db, sql);

        delete_stmt.bind_text("@id_token_hash", id_token_hash);

        if (delete_stmt.step() != SQLITE_DONE) {
            EVLOG_error << "Could not delete from table: " << sqlite3_errmsg(this->db);
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Exception while deleting from auth cache table: " << e.what();
    }
}

bool DatabaseHandler::clear_authorization_cache() {
    return this->clear_table("AUTH_CACHE");
}

void DatabaseHandler::insert_availability(const int32_t evse_id, std::optional<int32_t> connector_id,
                                          const OperationalStatusEnum& operational_status, const bool replace) {
    std::string sql = "INSERT OR REPLACE INTO AVAILABILITY (EVSE_ID, CONNECTOR_ID, OPERATIONAL_STATUS) VALUES "
                      "(@evse_id, @connector_id, @operational_status)";

    if (replace) {
        const std::string or_replace = "OR REPLACE";
        const std::string or_ignore = "OR IGNORE";
        size_t pos = sql.find(or_replace);
        sql.replace(pos, or_replace.length(), or_ignore);
    }

    SQLiteStatement insert_stmt(this->db, sql);

    insert_stmt.bind_int("@evse_id", evse_id);

    if (connector_id.has_value()) {
        insert_stmt.bind_int("@connector_id", connector_id.value());
    } else {
        insert_stmt.bind_null("@connector_id");
    }
    insert_stmt.bind_text("@operational_status", conversions::operational_status_enum_to_string(operational_status),
                          SQLiteString::Transient);

    if (insert_stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into AVAILABILITY table: " << sqlite3_errmsg(db);
        return;
    }
}

OperationalStatusEnum DatabaseHandler::get_availability(const int32_t evse_id, std::optional<int32_t> connector_id) {
    try {
        std::string sql =
            "SELECT OPERATIONAL_STATUS FROM AVAILABILITY WHERE EVSE_ID = @evse_id AND CONNECTOR_ID = @connector_id;";
        SQLiteStatement select_stmt(this->db, sql);

        select_stmt.bind_int("@evse_id", evse_id);

        if (connector_id.has_value()) {
            select_stmt.bind_int("@connector_id", connector_id.value());
        } else {
            select_stmt.bind_null("@connector_id");
        }

        if (select_stmt.step() != SQLITE_ROW) {
            throw std::runtime_error("Could not get availability from database");
        }
        return conversions::string_to_operational_status_enum(select_stmt.column_text(0));
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not get availability from database");
    }
}

} // namespace v201
} // namespace ocpp
