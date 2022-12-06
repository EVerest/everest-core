// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>

#include "ocpp1_6/database_handler.hpp"

namespace ocpp1_6 {

DatabaseHandler::DatabaseHandler(const std::string& chargepoint_id, const boost::filesystem::path& database_path,
                                 const boost::filesystem::path& init_script_path) {
    const auto sqlite_db_filename = chargepoint_id + ".db";
    if (!boost::filesystem::exists(database_path)) {
        boost::filesystem::create_directories(database_path);
    }
    this->db_path = database_path / sqlite_db_filename;
    this->init_script_path = init_script_path;
}

DatabaseHandler::~DatabaseHandler() {
    if (sqlite3_close_v2(this->db) == SQLITE_OK) {
        EVLOG_debug << "Successfully closed database file";
    } else {
        EVLOG_error << "Error closing database file: " << sqlite3_errmsg(this->db);
    }
}

void DatabaseHandler::open_db_connection(int32_t number_of_connectors) {
    if (sqlite3_open(this->db_path.c_str(), &this->db) != SQLITE_OK) {
        EVLOG_error << "Error opening database: " << sqlite3_errmsg(db);
        throw std::runtime_error("Could not open database at provided path.");
    }
    EVLOG_debug << "Established connection to Database.";
    this->run_sql_init();
    this->init_connector_table(number_of_connectors);
    this->insert_or_update_local_list_version(0);
}

void DatabaseHandler::close_db_connection() {
    if (sqlite3_close(this->db) == SQLITE_OK) {
        EVLOG_debug << "Successfully closed database file";
    } else {
        EVLOG_error << "Error closing database file: " << sqlite3_errmsg(this->db);
    }
}

void DatabaseHandler::run_sql_init() {
    EVLOG_debug << "Running SQL initialization script.";
    std::ifstream t(this->init_script_path.string());
    std::stringstream init_sql;

    init_sql << t.rdbuf();

    char* err = NULL;

    if (sqlite3_exec(this->db, init_sql.str().c_str(), NULL, NULL, &err) != SQLITE_OK) {
        EVLOG_error << "Could not create tables: " << err << std::endl;
        throw std::runtime_error("Database access error");
    }
}

void DatabaseHandler::init_connector_table(int32_t number_of_connectors) {
    for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
        std::ostringstream insert_sql;
        std::string sql = "INSERT OR IGNORE INTO CONNECTORS (ID, AVAILABILITY) VALUES (@connector, @availability_type)";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
            EVLOG_error << "Could not prepare insert statement" << std::endl;
            throw std::runtime_error("Database access error");
        }
        sqlite3_bind_int(stmt, 1, connector);
        const std::string operative_str("Operative");
        sqlite3_bind_text(stmt, 2, operative_str.c_str(), operative_str.length(), NULL);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db) << std::endl;
            throw std::runtime_error("db access error");
        }

        if (sqlite3_finalize(stmt) != SQLITE_OK) {
            EVLOG_error << "Error inserting into table" << std::endl;
            throw std::runtime_error("db access error");
        }
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

// transactions
void DatabaseHandler::insert_transaction(const std::string& session_id, const int32_t transaction_id,
                                         const int32_t connector, const std::string& id_tag_start,
                                         const std::string& time_start, const int32_t meter_start,
                                         const boost::optional<int32_t> reservation_id) {
    std::string sql =
        "INSERT INTO TRANSACTIONS (ID, TRANSACTION_ID, CONNECTOR, ID_TAG_START, TIME_START, METER_START, "
        "RESERVATION_ID) VALUES "
        "(@session_id, @transaction_id, @connector, @id_tag_start, @time_start, @meter_start, @reservation_id)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_warning << "Could not prepare insert statement" << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, session_id.c_str(), session_id.length(), NULL);
    sqlite3_bind_int(stmt, 2, transaction_id);
    sqlite3_bind_int(stmt, 3, connector);
    sqlite3_bind_text(stmt, 4, id_tag_start.c_str(), id_tag_start.length(), NULL);
    sqlite3_bind_text(stmt, 5, time_start.c_str(), time_start.length(), NULL);
    sqlite3_bind_int(stmt, 6, meter_start);

    if (reservation_id) {
        sqlite3_bind_int(stmt, 7, reservation_id.value());
    } else {
        sqlite3_bind_null(stmt, 7);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db) << std::endl;
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into table" << std::endl;
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::update_transaction(const std::string& session_id, int32_t transaction_id,
                                         boost::optional<ocpp1_6::CiString20Type> parent_id_tag) {

    std::string sql =
        "UPDATE TRANSACTIONS SET TRANSACTION_ID=@transaction_id, PARENT_ID_TAG=@parent_id_tag WHERE ID==@session_id";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_warning << "Could not prepare insert statement" << std::endl;
        return;
    }

    // bindings
    sqlite3_bind_int(stmt, 1, transaction_id);
    if (parent_id_tag != boost::none) {
        const std::string parent_id_tag_str(parent_id_tag.value().get());
        sqlite3_bind_text(stmt, 2, parent_id_tag_str.c_str(), parent_id_tag_str.length(), NULL);
    }
    sqlite3_bind_text(stmt, 3, session_id.c_str(), session_id.length(), NULL);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db) << std::endl;
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error updating table" << std::endl;
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::update_transaction(const std::string& session_id, int32_t meter_stop, const std::string& time_end,
                                         boost::optional<CiString20Type> id_tag_end,
                                         boost::optional<Reason> stop_reason) {
    std::string sql = "UPDATE TRANSACTIONS SET METER_STOP=@meter_stop, TIME_END=@time_end, "
                      "ID_TAG_END=@id_tag_end, STOP_REASON=@stop_reason WHERE ID==@session_id";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_warning << "Could not prepare insert statement" << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, meter_stop);
    sqlite3_bind_text(stmt, 2, time_end.c_str(), time_end.length(), NULL);
    if (id_tag_end != boost::none) {
        const std::string id_tag_end_str(id_tag_end.value().get());
        sqlite3_bind_text(stmt, 3, id_tag_end_str.c_str(), id_tag_end_str.length(), SQLITE_TRANSIENT);
    }
    if (stop_reason != boost::none) {
        const std::string stop_reason_str(conversions::reason_to_string(stop_reason.value()));
        sqlite3_bind_text(stmt, 4, stop_reason_str.c_str(), stop_reason_str.length(), SQLITE_TRANSIENT);
    }
    sqlite3_bind_text(stmt, 5, session_id.c_str(), session_id.length(), NULL);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db) << std::endl;
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error updating table" << std::endl;
        throw std::runtime_error("db access error");
    }
}

std::vector<TransactionEntry> DatabaseHandler::get_transactions(bool filter_incomplete) {
    std::vector<TransactionEntry> transactions;

    std::string sql = "SELECT * FROM TRANSACTIONS";

    if (filter_incomplete) {
        sql += " WHERE METER_STOP IS NULL OR TIME_END IS NULL";
    }
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement" << std::endl;
        throw std::runtime_error("Database access error");
    }

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        TransactionEntry transaction_entry;
        transaction_entry.session_id = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        transaction_entry.transaction_id = sqlite3_column_int(stmt, 1);
        transaction_entry.connector = sqlite3_column_int(stmt, 2);
        transaction_entry.id_tag_start = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        transaction_entry.time_start = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        transaction_entry.meter_start = sqlite3_column_int(stmt, 5);

        if (sqlite3_column_type(stmt, 6) != SQLITE_NULL) {
            transaction_entry.reservation_id.emplace(sqlite3_column_int(stmt, 6));
        }
        if (sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
            transaction_entry.parent_id_tag.emplace(
                std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7))));
        }
        if (sqlite3_column_type(stmt, 8) != SQLITE_NULL) {
            transaction_entry.id_tag_end.emplace(
                std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8))));
        }
        if (sqlite3_column_type(stmt, 9) != SQLITE_NULL) {
            transaction_entry.time_end.emplace(
                std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9))));
        }
        if (sqlite3_column_type(stmt, 10) != SQLITE_NULL) {
            transaction_entry.meter_stop.emplace(sqlite3_column_int(stmt, 10));
        }
        if (sqlite3_column_type(stmt, 11) != SQLITE_NULL) {
            transaction_entry.stop_reason.emplace(
                std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11))));
        }
        if (sqlite3_column_type(stmt, 12) != SQLITE_NULL) {
            transaction_entry.reservation_id.emplace(sqlite3_column_int(stmt, 12));
        }
        transactions.push_back(transaction_entry);
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error selecting from table" << std::endl;
        throw std::runtime_error("db access error");
    }

    return transactions;
}

// authorization cache
void DatabaseHandler::insert_or_update_authorization_cache_entry(const CiString20Type& id_tag,
                                                                 const IdTagInfo& id_tag_info) {

    // TODO(piet): Only call this when authorization cache is enabled!

    std::string sql = "INSERT OR REPLACE INTO AUTH_CACHE (ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG) VALUES "
                      "(@id_tag, @auth_status, @expiry_date, @parent_id_tag)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement" << std::endl;
        return;
    }

    const auto id_tag_str = id_tag.get();
    const auto status = ocpp1_6::conversions::authorization_status_to_string(id_tag_info.status);
    sqlite3_bind_text(stmt, 1, id_tag_str.c_str(), id_tag_str.length(), NULL);
    sqlite3_bind_text(stmt, 2, status.c_str(), status.length(), NULL);
    if (id_tag_info.expiryDate) {
        auto expiry_date = id_tag_info.expiryDate.value().to_rfc3339();
        sqlite3_bind_text(stmt, 3, expiry_date.c_str(), expiry_date.length(), SQLITE_TRANSIENT);
    }
    if (id_tag_info.parentIdTag) {
        const auto parent_id_tag = id_tag_info.parentIdTag.value().get();
        sqlite3_bind_text(stmt, 4, parent_id_tag.c_str(), parent_id_tag.length(), SQLITE_TRANSIENT);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into table";
        throw std::runtime_error("db access error");
    }
}

boost::optional<IdTagInfo> DatabaseHandler::get_authorization_cache_entry(const CiString20Type& id_tag) {

    // TODO(piet): Only call this when authorization cache is enabled!

    std::string sql = "SELECT ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG FROM AUTH_CACHE WHERE ID_TAG = @id_tag";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement" << std::endl;
        return boost::none;
    }

    const auto id_tag_str = id_tag.get();
    sqlite3_bind_text(stmt, 1, id_tag_str.c_str(), id_tag_str.length(), NULL);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        return boost::none;
    }

    IdTagInfo id_tag_info;
    const auto auth_status_ptr = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    id_tag_info.status = conversions::string_to_authorization_status(auth_status_ptr);

    auto expiry_date_ptr = sqlite3_column_text(stmt, 2);
    if (expiry_date_ptr != nullptr) {
        const auto expiry_date_str = std::string(reinterpret_cast<const char*>(expiry_date_ptr));
        const auto expiry_date = DateTime(expiry_date_str);
        id_tag_info.expiryDate.emplace(expiry_date);
    }

    const auto parent_id_tag_ptr = sqlite3_column_text(stmt, 3);
    if (parent_id_tag_ptr != nullptr) {
        const auto parent_id_tag_str = std::string(reinterpret_cast<const char*>(parent_id_tag_ptr));
        id_tag_info.parentIdTag.emplace(parent_id_tag_str);
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error selecting from table";
        throw std::runtime_error("db access error");
    }

    // check if expiry date is set and the entry should be set to Expired
    if (id_tag_info.status != AuthorizationStatus::Expired) {
        if (id_tag_info.expiryDate) {
            auto now = DateTime();
            if (id_tag_info.expiryDate.get() <= now) {
                EVLOG_debug << "IdTag " << id_tag
                            << " in auth cache has expiry date in the past, setting entry to expired.";
                id_tag_info.status = AuthorizationStatus::Expired;
                this->insert_or_update_authorization_cache_entry(id_tag, id_tag_info);
            }
        }
    }
    return id_tag_info;
}

bool DatabaseHandler::clear_authorization_cache() {
    // TODO(piet): Only call this when authorization cache is enabled!
    return this->clear_table("AUTH_CACHE");
}

void DatabaseHandler::insert_or_update_connector_availability(int32_t connector,
                                                              const AvailabilityType& availability_type) {
    std::string sql = "INSERT OR REPLACE INTO CONNECTORS (ID, AVAILABILITY) VALUES (@id, @availability)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement" << std::endl;
        throw std::runtime_error("Database access error");
    }
    sqlite3_bind_int(stmt, 1, connector);
    const std::string availability_type_str(conversions::availability_type_to_string(availability_type));
    sqlite3_bind_text(stmt, 2, availability_type_str.c_str(), availability_type_str.length(), NULL);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into table";
        throw std::runtime_error("db access error");
    }
}

// connector availability
void DatabaseHandler::insert_or_update_connector_availability(const std::vector<int32_t>& connectors,
                                                              const AvailabilityType& availability_type) {
    for (const auto connector : connectors) {
        this->insert_or_update_connector_availability(connector, availability_type);
    }
}

AvailabilityType DatabaseHandler::get_connector_availability(int32_t connector) {
    std::string sql = "SELECT AVAILABILITY FROM CONNECTORS WHERE ID = @connector";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement";
        throw std::runtime_error("Database access error");
    }

    sqlite3_bind_int(stmt, 1, connector);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        EVLOG_error << "Error selecting availability of connector with id " << connector;
        throw std::runtime_error("db access error");
    }

    const auto availability_str = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    AvailabilityType availability = conversions::string_to_availability_type(availability_str);

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into table";
        throw std::runtime_error("db access error");
    }

    return availability;
}

std::map<int32_t, AvailabilityType> DatabaseHandler::get_connector_availability() {
    std::map<int32_t, AvailabilityType> availability_map;
    const std::string sql = "SELECT ID, AVAILABILITY FROM CONNECTORS";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement";
        throw std::runtime_error("Database access error");
    }

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        auto connector = sqlite3_column_int(stmt, 0);
        auto availability_str = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        availability_map[connector] = conversions::string_to_availability_type(availability_str);
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error selecting from table" << std::endl;
        throw std::runtime_error("db access error");
    }

    return availability_map;
}

// local auth list management
void DatabaseHandler::insert_or_update_local_list_version(int32_t version) {
    std::string sql = "INSERT OR REPLACE INTO AUTH_LIST_VERSION (ID, VERSION) VALUES (0, @version)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement";
        throw std::runtime_error("Database access error");
    }
    sqlite3_bind_int(stmt, 1, version);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into table";
        throw std::runtime_error("db access error");
    }
}

int32_t DatabaseHandler::get_local_list_version() {
    std::string sql = "SELECT VERSION FROM AUTH_LIST_VERSION WHERE ID = 0";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement";
        throw std::runtime_error("Database access error");
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        EVLOG_error << "Error selecting auth list version";
        throw std::runtime_error("db access error");
    }

    int32_t version = sqlite3_column_int(stmt, 0);

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into table";
        throw std::runtime_error("db access error");
    }

    return version;
}

void DatabaseHandler::insert_or_update_local_authorization_list_entry(const CiString20Type& id_tag,
                                                                      const IdTagInfo& id_tag_info) {
    const auto id_tag_str = id_tag.get();
    // add or replace
    std::string insert_sql =
        "INSERT OR REPLACE INTO AUTH_LIST (ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG) VALUES "
        "(@id_tag, @auth_status, @expiry_date, @parent_id_tag)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, insert_sql.c_str(), insert_sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement";
        throw std::runtime_error("Database access error");
    }
    const auto status_str = ocpp1_6::conversions::authorization_status_to_string(id_tag_info.status);
    sqlite3_bind_text(stmt, 1, id_tag_str.c_str(), id_tag_str.length(), NULL);
    sqlite3_bind_text(stmt, 2, status_str.c_str(), status_str.length(), NULL);
    if (id_tag_info.expiryDate) {
        const auto expiry_date_str = id_tag_info.expiryDate.value().to_rfc3339();
        sqlite3_bind_text(stmt, 3, expiry_date_str.c_str(), expiry_date_str.length(), SQLITE_TRANSIENT);
    }
    if (id_tag_info.parentIdTag) {
        const auto parent_id_tag_str = id_tag_info.parentIdTag.value().get();
        sqlite3_bind_text(stmt, 4, parent_id_tag_str.c_str(), parent_id_tag_str.length(), SQLITE_TRANSIENT);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into table";
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::insert_or_update_local_authorization_list(
    std::vector<LocalAuthorizationList> local_authorization_list) {
    for (const auto& authorization_data : local_authorization_list) {
        if (authorization_data.idTagInfo) {
            this->insert_or_update_local_authorization_list_entry(authorization_data.idTag,
                                                                  authorization_data.idTagInfo.value());
        } else {
            this->delete_local_authorization_list_entry(authorization_data.idTag.get().c_str());
        }
    }
}

void DatabaseHandler::delete_local_authorization_list_entry(const std::string& id_tag) {
    std::string delete_sql = "DELETE FROM AUTH_LIST WHERE ID_TAG = @id_tag;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, delete_sql.c_str(), delete_sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement";
        throw std::runtime_error("Database access error");
    }
    sqlite3_bind_text(stmt, 1, id_tag.c_str(), id_tag.length(), NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not delete from table: " << sqlite3_errmsg(db);
    }
    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error deleting from table";
    }
}

boost::optional<IdTagInfo> DatabaseHandler::get_local_authorization_list_entry(const CiString20Type& id_tag) {
    std::string sql = "SELECT ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG FROM AUTH_LIST WHERE ID_TAG = @id_tag";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement" << std::endl;
        return boost::none;
    }

    const auto id_tag_str = id_tag.get();
    sqlite3_bind_text(stmt, 1, id_tag_str.c_str(), id_tag_str.length(), NULL);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        return boost::none;
    }

    IdTagInfo id_tag_info;
    const auto auth_status_ptr = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    id_tag_info.status = conversions::string_to_authorization_status(auth_status_ptr);

    auto expiry_date_ptr = sqlite3_column_text(stmt, 2);
    if (expiry_date_ptr != nullptr) {
        const auto expiry_date_str = std::string(reinterpret_cast<const char*>(expiry_date_ptr));
        const auto expiry_date = DateTime(expiry_date_str);
        id_tag_info.expiryDate.emplace(expiry_date);
    }

    const auto parent_id_tag_ptr = sqlite3_column_text(stmt, 3);
    if (parent_id_tag_ptr != nullptr) {
        const auto parent_id_tag_str = std::string(reinterpret_cast<const char*>(parent_id_tag_ptr));
        id_tag_info.parentIdTag.emplace(parent_id_tag_str);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error selecting from table";
        throw std::runtime_error("db access error");
    }

    // check if expiry date is set and the entry should be set to Expired
    if (id_tag_info.status != AuthorizationStatus::Expired) {
        if (id_tag_info.expiryDate) {
            auto now = DateTime();
            if (id_tag_info.expiryDate.get() <= now) {
                EVLOG_debug << "IdTag " << id_tag
                            << " in auth list has expiry date in the past, setting entry to expired.";
                id_tag_info.status = AuthorizationStatus::Expired;
                this->insert_or_update_local_authorization_list_entry(id_tag, id_tag_info);
            }
        }
    }
    return id_tag_info;
}

bool DatabaseHandler::clear_local_authorization_list() {
    return this->clear_table("AUTH_LIST");
}

void DatabaseHandler::insert_or_update_charging_profile(const int connector_id, const ChargingProfile& profile) {
    // add or replace
    std::string insert_sql = "INSERT OR REPLACE INTO CHARGING_PROFILES (ID, CONNECTOR_ID, PROFILE) VALUES "
                             "(@id, @connector_id, @profile)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, insert_sql.c_str(), insert_sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_error << "Could not prepare insert statement";
        throw std::runtime_error("Database access error");
    }

    json json_profile(profile);

    sqlite3_bind_int(stmt, 1, profile.chargingProfileId);
    sqlite3_bind_int(stmt, 2, connector_id);
    sqlite3_bind_text(stmt, 3, json_profile.dump().c_str(), json_profile.dump().length(), SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error inserting into table";
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::delete_charging_profile(const int profile_id) {
    std::string delete_sql = "DELETE FROM CHARGING_PROFILES WHERE ID = @id;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, delete_sql.c_str(), delete_sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_warning << "Could not prepare insert statement";
        return;
    }
    sqlite3_bind_int(stmt, 1, profile_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        EVLOG_error << "Could not delete from table: " << sqlite3_errmsg(db);
    }
    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_error << "Error deleting from table";
    }
}

void DatabaseHandler::delete_charging_profiles() {
    this->clear_table("CHARGING_PROFILES");
}

std::vector<ChargingProfile> DatabaseHandler::get_charging_profiles() {

    std::vector<ChargingProfile> profiles;
    std::string sql = "SELECT * FROM CHARGING_PROFILES";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_warning << "Could not prepare insert statement" << std::endl;
        return profiles;
    }

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        ChargingProfile profile(json::parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)))));
        profiles.push_back(profile);
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        EVLOG_warning << "Error selecting from table" << std::endl;
        return profiles;
    }
    return profiles;
}

int DatabaseHandler::get_connector_id(const int profile_id) {

    std::string sql = "SELECT CONNECTOR_ID FROM CHARGING_PROFILES WHERE ID = @profile_id";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        EVLOG_warning << "Could not prepare select statement" << std::endl;
        return -1;
    }

    sqlite3_bind_int(stmt, 1, profile_id);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        EVLOG_warning << "Requesting an unknown profile_id from database";
        return -1;
    }

    const auto connector_id = sqlite3_column_int(stmt, 0);
    return connector_id;
}

} // namespace ocpp1_6
