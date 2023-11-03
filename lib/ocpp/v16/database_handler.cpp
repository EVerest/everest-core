// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>

#include <ocpp/common/sqlite_statement.hpp>
#include <ocpp/v16/database_handler.hpp>

namespace ocpp {
namespace v16 {

DatabaseHandler::DatabaseHandler(const std::string& chargepoint_id, const fs::path& database_path,
                                 const fs::path& init_script_path) :
    ocpp::common::DatabaseHandlerBase() {
    const auto sqlite_db_filename = chargepoint_id + ".db";
    if (!fs::exists(database_path)) {
        fs::create_directories(database_path);
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
        EVLOG_error << "Could not create tables: " << sqlite3_errmsg(this->db);
        throw std::runtime_error("Database access error");
    }
}

void DatabaseHandler::init_connector_table(int32_t number_of_connectors) {
    for (int32_t connector = 0; connector <= number_of_connectors; connector++) {
        std::string sql = "INSERT OR IGNORE INTO CONNECTORS (ID, AVAILABILITY) VALUES (@connector, @availability_type)";
        SQLiteStatement stmt(this->db, sql);

        stmt.bind_int("@connector", connector);
        stmt.bind_text("@availability_type", "Operative", SQLiteString::Transient);

        if (stmt.step() != SQLITE_DONE) {
            EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db);
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
                                         const std::string& time_start, const int32_t meter_start, const bool csms_ack,
                                         const std::optional<int32_t> reservation_id) {
    std::string sql = "INSERT INTO TRANSACTIONS (ID, TRANSACTION_ID, CONNECTOR, ID_TAG_START, TIME_START, METER_START, "
                      "CSMS_ACK, METER_LAST, METER_LAST_TIME, LAST_UPDATE, RESERVATION_ID) VALUES "
                      "(@session_id, @transaction_id, @connector, @id_tag_start, @time_start, @meter_start, @csms_ack, "
                      "@meter_last, @meter_last_time, @last_update, @reservation_id)";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_text("@session_id", session_id);
    stmt.bind_int("@transaction_id", transaction_id);
    stmt.bind_int("@connector", connector);
    stmt.bind_text("@id_tag_start", id_tag_start);
    stmt.bind_text("@time_start", time_start);
    stmt.bind_int("@meter_start", meter_start);
    stmt.bind_int("@csms_ack", int(csms_ack));
    stmt.bind_int("@meter_last", meter_start);
    stmt.bind_text("@meter_last_time", time_start);
    stmt.bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);

    if (reservation_id.has_value()) {
        stmt.bind_int("@reservation_id", reservation_id.value());
    } else {
        stmt.bind_null("@reservation_id");
    }

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db) << std::endl;
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::update_transaction(const std::string& session_id, int32_t transaction_id,
                                         std::optional<CiString<20>> parent_id_tag) {

    std::string sql = "UPDATE TRANSACTIONS SET TRANSACTION_ID=@transaction_id, PARENT_ID_TAG=@parent_id_tag, "
                      "LAST_UPDATE=@last_update WHERE ID==@session_id";
    SQLiteStatement stmt(this->db, sql);

    // bindings
    stmt.bind_int("@transaction_id", transaction_id);
    if (parent_id_tag.has_value()) {
        stmt.bind_text("@parent_id_tag", parent_id_tag.value().get(), SQLiteString::Transient);
    }
    stmt.bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt.bind_text("@session_id", session_id);

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db) << std::endl;
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::update_transaction(const std::string& session_id, int32_t meter_stop, const std::string& time_end,
                                         std::optional<CiString<20>> id_tag_end,
                                         std::optional<v16::Reason> stop_reason) {
    std::string sql =
        "UPDATE TRANSACTIONS SET METER_STOP=@meter_stop, TIME_END=@time_end, "
        "ID_TAG_END=@id_tag_end, STOP_REASON=@stop_reason, LAST_UPDATE=@last_update WHERE ID==@session_id";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_int("@meter_stop", meter_stop);
    stmt.bind_text("@time_end", time_end);
    if (id_tag_end.has_value()) {
        stmt.bind_text("@id_tag_end", id_tag_end.value().get(), SQLiteString::Transient);
    }
    if (stop_reason.has_value()) {
        stmt.bind_text("@stop_reason", v16::conversions::reason_to_string(stop_reason.value()),
                       SQLiteString::Transient);
    }
    stmt.bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt.bind_text("@session_id", session_id);

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db) << std::endl;
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::update_transaction_csms_ack(const int32_t transaction_id) {
    std::string sql =
        "UPDATE TRANSACTIONS SET CSMS_ACK=1, LAST_UPDATE=@last_update WHERE TRANSACTION_ID==@transaction_id";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt.bind_int("@transaction_id", transaction_id);

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db) << std::endl;
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::update_transaction_meter_value(const std::string& session_id, const int32_t value,
                                                     const std::string& last_meter_time) {
    std::string sql = "UPDATE TRANSACTIONS SET METER_LAST=@meter_last, METER_LAST_TIME=@meter_last_time, "
                      "LAST_UPDATE=@last_update WHERE ID==@session_id";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_int("@meter_last", value);
    stmt.bind_text("@meter_last_time", last_meter_time);
    stmt.bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt.bind_text("@session_id", session_id);

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(this->db);
        throw std::runtime_error("db access error");
    }
}

std::vector<TransactionEntry> DatabaseHandler::get_transactions(bool filter_incomplete) {
    std::vector<TransactionEntry> transactions;

    std::string sql = "SELECT * FROM TRANSACTIONS";

    if (filter_incomplete) {
        sql += " WHERE CSMS_ACK==0";
    }

    SQLiteStatement stmt(this->db, sql);

    while (stmt.step() != SQLITE_DONE) {
        TransactionEntry transaction_entry;
        transaction_entry.session_id = stmt.column_text(0);
        transaction_entry.transaction_id = stmt.column_int(1);
        transaction_entry.connector = stmt.column_int(2);
        transaction_entry.id_tag_start = stmt.column_text(3);
        transaction_entry.time_start = stmt.column_text(4);
        transaction_entry.meter_start = stmt.column_int(5);
        transaction_entry.csms_ack = bool(stmt.column_int(6));
        transaction_entry.meter_last = stmt.column_int(7);
        transaction_entry.meter_last_time = stmt.column_text(8);
        transaction_entry.last_update = stmt.column_text(9);

        if (stmt.column_type(10) != SQLITE_NULL) {
            transaction_entry.reservation_id.emplace(stmt.column_int(10));
        }
        if (stmt.column_type(11) != SQLITE_NULL) {
            transaction_entry.parent_id_tag.emplace(stmt.column_text(11));
        }
        if (stmt.column_type(12) != SQLITE_NULL) {
            transaction_entry.id_tag_end.emplace(stmt.column_text(12));
        }
        if (stmt.column_type(13) != SQLITE_NULL) {
            transaction_entry.time_end.emplace(stmt.column_text(13));
        }
        if (stmt.column_type(14) != SQLITE_NULL) {
            transaction_entry.meter_stop.emplace(stmt.column_int(14));
        }
        if (stmt.column_type(15) != SQLITE_NULL) {
            transaction_entry.stop_reason.emplace(stmt.column_text(15));
        }
        if (stmt.column_type(16) != SQLITE_NULL) {
            transaction_entry.reservation_id.emplace(stmt.column_int(16));
        }
        transactions.push_back(transaction_entry);
    }

    return transactions;
}

// authorization cache
void DatabaseHandler::insert_or_update_authorization_cache_entry(const CiString<20>& id_tag,
                                                                 const v16::IdTagInfo& id_tag_info) {

    // TODO(piet): Only call this when authorization cache is enabled!

    std::string sql = "INSERT OR REPLACE INTO AUTH_CACHE (ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG) VALUES "
                      "(@id_tag, @auth_status, @expiry_date, @parent_id_tag)";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_text("@id_tag", id_tag.get(), SQLiteString::Transient);
    stmt.bind_text("@auth_status", v16::conversions::authorization_status_to_string(id_tag_info.status),
                   SQLiteString::Transient);
    if (id_tag_info.expiryDate.has_value()) {
        stmt.bind_text("@expiry_date", id_tag_info.expiryDate.value().to_rfc3339(), SQLiteString::Transient);
    }
    if (id_tag_info.parentIdTag.has_value()) {
        stmt.bind_text("@parent_id_tag", id_tag_info.parentIdTag.value().get(), SQLiteString::Transient);
    }

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }
}

std::optional<v16::IdTagInfo> DatabaseHandler::get_authorization_cache_entry(const CiString<20>& id_tag) {

    // TODO(piet): Only call this when authorization cache is enabled!

    std::string sql = "SELECT ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG FROM AUTH_CACHE WHERE ID_TAG = @id_tag";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_text("@id_tag", id_tag.get(), SQLiteString::Transient);

    if (stmt.step() != SQLITE_ROW) {
        return std::nullopt;
    }

    v16::IdTagInfo id_tag_info;
    id_tag_info.status = v16::conversions::string_to_authorization_status(stmt.column_text(1));

    if (stmt.column_type(2) != SQLITE_NULL) {
        id_tag_info.expiryDate.emplace(stmt.column_text(2));
    }

    if (stmt.column_type(3) != SQLITE_NULL) {
        id_tag_info.parentIdTag.emplace(stmt.column_text(3));
    }

    // check if expiry date is set and the entry should be set to Expired
    if (id_tag_info.status != v16::AuthorizationStatus::Expired) {
        if (id_tag_info.expiryDate) {
            auto now = DateTime();
            if (id_tag_info.expiryDate.value() <= now) {
                EVLOG_debug << "IdTag " << id_tag
                            << " in auth cache has expiry date in the past, setting entry to expired.";
                id_tag_info.status = v16::AuthorizationStatus::Expired;
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
                                                              const v16::AvailabilityType& availability_type) {
    std::string sql = "INSERT OR REPLACE INTO CONNECTORS (ID, AVAILABILITY) VALUES (@id, @availability)";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_int("@id", connector);
    stmt.bind_text("@availability", v16::conversions::availability_type_to_string(availability_type),
                   SQLiteString::Transient);

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }
}

// connector availability
void DatabaseHandler::insert_or_update_connector_availability(const std::vector<int32_t>& connectors,
                                                              const v16::AvailabilityType& availability_type) {
    for (const auto connector : connectors) {
        this->insert_or_update_connector_availability(connector, availability_type);
    }
}

v16::AvailabilityType DatabaseHandler::get_connector_availability(int32_t connector) {
    std::string sql = "SELECT AVAILABILITY FROM CONNECTORS WHERE ID = @connector";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_int("@connector", connector);
    if (stmt.step() != SQLITE_ROW) {
        EVLOG_error << "Error selecting availability of connector: " << sqlite3_errmsg(this->db);
        throw std::runtime_error("db access error");
    }

    return v16::conversions::string_to_availability_type(stmt.column_text(0));
}

std::map<int32_t, v16::AvailabilityType> DatabaseHandler::get_connector_availability() {
    std::map<int32_t, v16::AvailabilityType> availability_map;
    const std::string sql = "SELECT ID, AVAILABILITY FROM CONNECTORS";
    SQLiteStatement stmt(this->db, sql);

    while (stmt.step() != SQLITE_DONE) {
        auto connector = stmt.column_int(0);
        availability_map[connector] = v16::conversions::string_to_availability_type(stmt.column_text(1));
    }

    return availability_map;
}

// local auth list management
void DatabaseHandler::insert_or_update_local_list_version(int32_t version) {
    std::string sql = "INSERT OR REPLACE INTO AUTH_LIST_VERSION (ID, VERSION) VALUES (0, @version)";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_int("@version", version);
    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }
}

int32_t DatabaseHandler::get_local_list_version() {
    std::string sql = "SELECT VERSION FROM AUTH_LIST_VERSION WHERE ID = 0";
    SQLiteStatement stmt(this->db, sql);

    if (stmt.step() != SQLITE_ROW) {
        EVLOG_error << "Error selecting auth list version: " << sqlite3_errmsg(this->db);
        throw std::runtime_error("db access error");
    }

    return stmt.column_int(0);
}

void DatabaseHandler::insert_or_update_local_authorization_list_entry(const CiString<20>& id_tag,
                                                                      const v16::IdTagInfo& id_tag_info) {
    // add or replace
    std::string sql = "INSERT OR REPLACE INTO AUTH_LIST (ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG) VALUES "
                      "(@id_tag, @auth_status, @expiry_date, @parent_id_tag)";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_text("@id_tag", id_tag.get(), SQLiteString::Transient);
    stmt.bind_text("@auth_status", v16::conversions::authorization_status_to_string(id_tag_info.status),
                   SQLiteString::Transient);
    if (id_tag_info.expiryDate.has_value()) {
        stmt.bind_text("@expiry_date", id_tag_info.expiryDate.value().to_rfc3339(), SQLiteString::Transient);
    }
    if (id_tag_info.parentIdTag.has_value()) {
        stmt.bind_text("@parent_id_tag", id_tag_info.parentIdTag.value().get(), SQLiteString::Transient);
    }

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::insert_or_update_local_authorization_list(
    std::vector<v16::LocalAuthorizationList> local_authorization_list) {
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
    std::string sql = "DELETE FROM AUTH_LIST WHERE ID_TAG = @id_tag;";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_text("@id_tag", id_tag);
    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not delete from table: " << sqlite3_errmsg(db);
    }
}

std::optional<v16::IdTagInfo> DatabaseHandler::get_local_authorization_list_entry(const CiString<20>& id_tag) {
    std::string sql = "SELECT ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG FROM AUTH_LIST WHERE ID_TAG = @id_tag";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_text("@id_tag", id_tag.get(), SQLiteString::Transient);

    if (stmt.step() != SQLITE_ROW) {
        return std::nullopt;
    }

    v16::IdTagInfo id_tag_info;
    id_tag_info.status = v16::conversions::string_to_authorization_status(stmt.column_text(1));

    if (stmt.column_type(2) != SQLITE_NULL) {
        id_tag_info.expiryDate.emplace(stmt.column_text(2));
    }

    if (stmt.column_type(3) != SQLITE_NULL) {
        id_tag_info.parentIdTag.emplace(stmt.column_text(3));
    }

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }

    // check if expiry date is set and the entry should be set to Expired
    if (id_tag_info.status != v16::AuthorizationStatus::Expired) {
        if (id_tag_info.expiryDate) {
            auto now = DateTime();
            if (id_tag_info.expiryDate.value() <= now) {
                EVLOG_debug << "IdTag " << id_tag
                            << " in auth list has expiry date in the past, setting entry to expired.";
                id_tag_info.status = v16::AuthorizationStatus::Expired;
                this->insert_or_update_local_authorization_list_entry(id_tag, id_tag_info);
            }
        }
    }
    return id_tag_info;
}

bool DatabaseHandler::clear_local_authorization_list() {
    return this->clear_table("AUTH_LIST");
}

void DatabaseHandler::insert_or_update_charging_profile(const int connector_id, const v16::ChargingProfile& profile) {
    // add or replace
    std::string sql = "INSERT OR REPLACE INTO CHARGING_PROFILES (ID, CONNECTOR_ID, PROFILE) VALUES "
                      "(@id, @connector_id, @profile)";
    SQLiteStatement stmt(this->db, sql);

    json json_profile(profile);

    stmt.bind_int("@id", profile.chargingProfileId);
    stmt.bind_int("@connector_id", connector_id);
    stmt.bind_text("@profile", json_profile.dump(), SQLiteString::Transient);

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::delete_charging_profile(const int profile_id) {
    std::string sql = "DELETE FROM CHARGING_PROFILES WHERE ID = @id;";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_int("@id", profile_id);
    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not delete from table: " << sqlite3_errmsg(db);
    }
}

void DatabaseHandler::delete_charging_profiles() {
    this->clear_table("CHARGING_PROFILES");
}

std::vector<v16::ChargingProfile> DatabaseHandler::get_charging_profiles() {

    std::vector<v16::ChargingProfile> profiles;
    std::string sql = "SELECT * FROM CHARGING_PROFILES";
    SQLiteStatement stmt(this->db, sql);

    while (stmt.step() != SQLITE_DONE) {
        profiles.emplace_back(json::parse(stmt.column_text(2)));
    }

    return profiles;
}

int DatabaseHandler::get_connector_id(const int profile_id) {
    std::string sql = "SELECT CONNECTOR_ID FROM CHARGING_PROFILES WHERE ID = @profile_id";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_int("@profile_id", profile_id);

    if (stmt.step() != SQLITE_ROW) {
        EVLOG_warning << "Requesting an unknown profile_id from database";
        return -1;
    }

    return stmt.column_int(0);
}

void DatabaseHandler::insert_ocsp_update() {
    std::string sql = "INSERT OR REPLACE INTO OCSP_REQUEST (LAST_UPDATE) VALUES "
                      "(@last_update)";
    SQLiteStatement stmt(this->db, sql);

    stmt.bind_text("@last_update", DateTime().to_rfc3339(), SQLiteString::Transient);

    if (stmt.step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }
}

std::optional<DateTime> DatabaseHandler::get_last_ocsp_update() {
    std::string sql = "SELECT LAST_UPDATE FROM OCSP_REQUEST";
    SQLiteStatement stmt(this->db, sql);

    if (stmt.step() != SQLITE_ROW) {
        return std::nullopt;
    }

    return DateTime(stmt.column_text(0));
}

} // namespace v16
} // namespace ocpp
