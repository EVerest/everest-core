// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>

#include <ocpp/v16/database_handler.hpp>

namespace ocpp {

using namespace common;

namespace v16 {

DatabaseHandler::DatabaseHandler(std::unique_ptr<DatabaseConnectionInterface> database,
                                 const fs::path& sql_migration_files_path, int32_t number_of_connectors) :
    DatabaseHandlerCommon(std::move(database), sql_migration_files_path, MIGRATION_FILE_VERSION_V16),
    number_of_connectors(number_of_connectors) {
}

void DatabaseHandler::init_sql() {
    this->init_connector_table();
    try {
        this->insert_or_ignore_local_list_version(0);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not insert or ignore version into AUTH_LIST_VERSION table: " << e.what();
    }
}

void DatabaseHandler::init_connector_table() {
    for (int32_t connector = 0; connector <= this->number_of_connectors; connector++) {
        std::string sql = "INSERT OR IGNORE INTO CONNECTORS (ID, AVAILABILITY) VALUES (@connector, @availability_type)";
        auto stmt = this->database->new_statement(sql);

        stmt->bind_int("@connector", connector);
        stmt->bind_text("@availability_type", "Operative", SQLiteString::Transient);

        if (stmt->step() != SQLITE_DONE) {
            EVLOG_error << "Could not insert into Connector table";
            throw QueryExecutionException(this->database->get_error_message());
        }
    }
}

// transactions
void DatabaseHandler::insert_transaction(const std::string& session_id, const int32_t transaction_id,
                                         const int32_t connector, const std::string& id_tag_start,
                                         const std::string& time_start, const int32_t meter_start, const bool csms_ack,
                                         const std::optional<int32_t> reservation_id,
                                         const std::string& start_transaction_message_id) {
    std::string sql =
        "INSERT INTO TRANSACTIONS (ID, TRANSACTION_ID, CONNECTOR, ID_TAG_START, TIME_START, METER_START, "
        "CSMS_ACK, METER_LAST, METER_LAST_TIME, LAST_UPDATE, RESERVATION_ID, START_TRANSACTION_MESSAGE_ID) VALUES "
        "(@session_id, @transaction_id, @connector, @id_tag_start, @time_start, @meter_start, @csms_ack, "
        "@meter_last, @meter_last_time, @last_update, @reservation_id, @start_transaction_message_id)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@session_id", session_id);
    stmt->bind_int("@transaction_id", transaction_id);
    stmt->bind_int("@connector", connector);
    stmt->bind_text("@id_tag_start", id_tag_start);
    stmt->bind_text("@time_start", time_start);
    stmt->bind_int("@meter_start", meter_start);
    stmt->bind_int("@csms_ack", int(csms_ack));
    stmt->bind_int("@meter_last", meter_start);
    stmt->bind_text("@meter_last_time", time_start);
    stmt->bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt->bind_text("@start_transaction_message_id", start_transaction_message_id, SQLiteString::Transient);

    if (reservation_id.has_value()) {
        stmt->bind_int("@reservation_id", reservation_id.value());
    } else {
        stmt->bind_null("@reservation_id");
    }

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::update_transaction(const std::string& session_id, int32_t transaction_id,
                                         std::optional<CiString<20>> parent_id_tag) {

    std::string sql = "UPDATE TRANSACTIONS SET TRANSACTION_ID=@transaction_id, PARENT_ID_TAG=@parent_id_tag, "
                      "LAST_UPDATE=@last_update WHERE ID==@session_id";
    auto stmt = this->database->new_statement(sql);

    // bindings
    stmt->bind_int("@transaction_id", transaction_id);
    if (parent_id_tag.has_value()) {
        stmt->bind_text("@parent_id_tag", parent_id_tag.value().get(), SQLiteString::Transient);
    }
    stmt->bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt->bind_text("@session_id", session_id);

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::update_transaction(const std::string& session_id, int32_t meter_stop, const std::string& time_end,
                                         std::optional<CiString<20>> id_tag_end, std::optional<v16::Reason> stop_reason,
                                         const std::string& stop_transaction_message_id) {
    std::string sql = "UPDATE TRANSACTIONS SET METER_STOP=@meter_stop, TIME_END=@time_end, "
                      "ID_TAG_END=@id_tag_end, STOP_REASON=@stop_reason, LAST_UPDATE=@last_update, "
                      "STOP_TRANSACTION_MESSAGE_ID=@stop_transaction_message_id WHERE ID==@session_id";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@meter_stop", meter_stop);
    stmt->bind_text("@time_end", time_end);
    if (id_tag_end.has_value()) {
        stmt->bind_text("@id_tag_end", id_tag_end.value().get(), SQLiteString::Transient);
    }
    if (stop_reason.has_value()) {
        stmt->bind_text("@stop_reason", v16::conversions::reason_to_string(stop_reason.value()),
                        SQLiteString::Transient);
    }
    stmt->bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt->bind_text("@session_id", session_id);
    stmt->bind_text("@stop_transaction_message_id", stop_transaction_message_id, SQLiteString::Transient);

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::update_transaction_csms_ack(const int32_t transaction_id) {
    std::string sql =
        "UPDATE TRANSACTIONS SET CSMS_ACK=1, LAST_UPDATE=@last_update WHERE TRANSACTION_ID==@transaction_id";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt->bind_int("@transaction_id", transaction_id);

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::update_transaction_meter_value(const std::string& session_id, const int32_t value,
                                                     const std::string& last_meter_time) {
    std::string sql = "UPDATE TRANSACTIONS SET METER_LAST=@meter_last, METER_LAST_TIME=@meter_last_time, "
                      "LAST_UPDATE=@last_update WHERE ID==@session_id";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@meter_last", value);
    stmt->bind_text("@meter_last_time", last_meter_time);
    stmt->bind_text("@last_update", ocpp::DateTime().to_rfc3339(), SQLiteString::Transient);
    stmt->bind_text("@session_id", session_id);

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

std::vector<TransactionEntry> DatabaseHandler::get_transactions(bool filter_incomplete) {
    std::vector<TransactionEntry> transactions;

    std::string sql = "SELECT * FROM TRANSACTIONS";

    if (filter_incomplete) {
        sql += " WHERE CSMS_ACK==0";
    }

    auto stmt = this->database->new_statement(sql);

    int status;
    while ((status = stmt->step()) == SQLITE_ROW) {
        TransactionEntry transaction_entry;
        transaction_entry.session_id = stmt->column_text(0);
        transaction_entry.transaction_id = stmt->column_int(1);
        transaction_entry.connector = stmt->column_int(2);
        transaction_entry.id_tag_start = stmt->column_text(3);
        transaction_entry.time_start = stmt->column_text(4);
        transaction_entry.meter_start = stmt->column_int(5);
        transaction_entry.csms_ack = bool(stmt->column_int(6));
        transaction_entry.meter_last = stmt->column_int(7);
        transaction_entry.meter_last_time = stmt->column_text(8);
        transaction_entry.last_update = stmt->column_text(9);

        if (stmt->column_type(10) != SQLITE_NULL) {
            transaction_entry.reservation_id.emplace(stmt->column_int(10));
        }
        if (stmt->column_type(11) != SQLITE_NULL) {
            transaction_entry.parent_id_tag.emplace(stmt->column_text(11));
        }
        if (stmt->column_type(12) != SQLITE_NULL) {
            transaction_entry.id_tag_end.emplace(stmt->column_text(12));
        }
        if (stmt->column_type(13) != SQLITE_NULL) {
            transaction_entry.time_end.emplace(stmt->column_text(13));
        }
        if (stmt->column_type(14) != SQLITE_NULL) {
            transaction_entry.meter_stop.emplace(stmt->column_int(14));
        }
        if (stmt->column_type(15) != SQLITE_NULL) {
            transaction_entry.stop_reason.emplace(stmt->column_text(15));
        }
        transaction_entry.start_transaction_message_id = stmt->column_text(16);
        if (stmt->column_type(17) != SQLITE_NULL) {
            transaction_entry.stop_transaction_message_id = stmt->column_text(17);
        }
        transactions.push_back(transaction_entry);
    }

    if (status != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }

    return transactions;
}

// authorization cache
void DatabaseHandler::insert_or_update_authorization_cache_entry(const CiString<20>& id_tag,
                                                                 const v16::IdTagInfo& id_tag_info) {

    // TODO(piet): Only call this when authorization cache is enabled!

    std::string sql = "INSERT OR REPLACE INTO AUTH_CACHE (ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG) VALUES "
                      "(@id_tag, @auth_status, @expiry_date, @parent_id_tag)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@id_tag", id_tag.get(), SQLiteString::Transient);
    stmt->bind_text("@auth_status", v16::conversions::authorization_status_to_string(id_tag_info.status),
                    SQLiteString::Transient);
    if (id_tag_info.expiryDate.has_value()) {
        stmt->bind_text("@expiry_date", id_tag_info.expiryDate.value().to_rfc3339(), SQLiteString::Transient);
    }
    if (id_tag_info.parentIdTag.has_value()) {
        stmt->bind_text("@parent_id_tag", id_tag_info.parentIdTag.value().get(), SQLiteString::Transient);
    }

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

std::optional<v16::IdTagInfo> DatabaseHandler::get_authorization_cache_entry(const CiString<20>& id_tag) {

    std::string sql = "SELECT ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG FROM AUTH_CACHE WHERE ID_TAG = @id_tag";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@id_tag", id_tag.get(), SQLiteString::Transient);

    int status = stmt->step();

    // entry was not found
    if (status == SQLITE_DONE) {
        return std::nullopt;
    }

    if (status == SQLITE_ROW) {
        v16::IdTagInfo id_tag_info;
        id_tag_info.status = v16::conversions::string_to_authorization_status(stmt->column_text(1));

        if (stmt->column_type(2) != SQLITE_NULL) {
            id_tag_info.expiryDate.emplace(stmt->column_text(2));
        }

        if (stmt->column_type(3) != SQLITE_NULL) {
            id_tag_info.parentIdTag.emplace(stmt->column_text(3));
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

    throw QueryExecutionException(this->database->get_error_message());
}

void DatabaseHandler::clear_authorization_cache() {
    const auto retval = this->database->clear_table("AUTH_CACHE");
    if (retval == false) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::insert_or_update_connector_availability(int32_t connector,
                                                              const v16::AvailabilityType& availability_type) {
    std::string sql = "INSERT OR REPLACE INTO CONNECTORS (ID, AVAILABILITY) VALUES (@id, @availability)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@id", connector);
    stmt->bind_text("@availability", v16::conversions::availability_type_to_string(availability_type),
                    SQLiteString::Transient);

    if (stmt->step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert availability into CONNECTORS table";
        throw QueryExecutionException(this->database->get_error_message());
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
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@connector", connector);

    int status = stmt->step();

    if (status == SQLITE_DONE) {
        throw RequiredEntryNotFoundException("Connector not found");
    }

    if (status != SQLITE_ROW) {
        EVLOG_error << "Error while selecting availability of CONNECTORS table";
        throw QueryExecutionException(this->database->get_error_message());
    }

    return v16::conversions::string_to_availability_type(stmt->column_text(0));
}

std::map<int32_t, v16::AvailabilityType> DatabaseHandler::get_connector_availability() {
    std::map<int32_t, v16::AvailabilityType> availability_map;
    const std::string sql = "SELECT ID, AVAILABILITY FROM CONNECTORS";
    auto stmt = this->database->new_statement(sql);

    int status;
    while ((status = stmt->step()) == SQLITE_ROW) {
        auto connector = stmt->column_int(0);
        availability_map[connector] = v16::conversions::string_to_availability_type(stmt->column_text(1));
    }

    if (status != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }

    return availability_map;
}

void DatabaseHandler::insert_or_ignore_local_list_version(int32_t version) {
    std::string sql = "INSERT OR IGNORE INTO AUTH_LIST_VERSION (ID, VERSION) VALUES (0, @version)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@version", version);
    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

// local auth list management
void DatabaseHandler::insert_or_update_local_list_version(int32_t version) {
    std::string sql = "INSERT OR REPLACE INTO AUTH_LIST_VERSION (ID, VERSION) VALUES (0, @version)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@version", version);
    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

int32_t DatabaseHandler::get_local_list_version() {
    std::string sql = "SELECT VERSION FROM AUTH_LIST_VERSION WHERE ID = 0";
    auto stmt = this->database->new_statement(sql);

    int status = stmt->step();
    if (status == SQLITE_DONE) {
        throw RequiredEntryNotFoundException("Local list version not found");
    }

    if (status != SQLITE_ROW) {
        throw QueryExecutionException(this->database->get_error_message());
    }

    return stmt->column_int(0);
}

void DatabaseHandler::insert_or_update_local_authorization_list_entry(const CiString<20>& id_tag,
                                                                      const v16::IdTagInfo& id_tag_info) {
    // add or replace
    std::string sql = "INSERT OR REPLACE INTO AUTH_LIST (ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG) VALUES "
                      "(@id_tag, @auth_status, @expiry_date, @parent_id_tag)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@id_tag", id_tag.get(), SQLiteString::Transient);
    stmt->bind_text("@auth_status", v16::conversions::authorization_status_to_string(id_tag_info.status),
                    SQLiteString::Transient);
    if (id_tag_info.expiryDate.has_value()) {
        stmt->bind_text("@expiry_date", id_tag_info.expiryDate.value().to_rfc3339(), SQLiteString::Transient);
    }
    if (id_tag_info.parentIdTag.has_value()) {
        stmt->bind_text("@parent_id_tag", id_tag_info.parentIdTag.value().get(), SQLiteString::Transient);
    }

    if (stmt->step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert or update local authorization list entry into the database";
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::insert_or_update_local_authorization_list(
    std::vector<v16::LocalAuthorizationList> local_authorization_list) {
    bool success = true; // indicates if all database operations succeeded
    for (const auto& authorization_data : local_authorization_list) {
        try {
            if (authorization_data.idTagInfo) {
                this->insert_or_update_local_authorization_list_entry(authorization_data.idTag,
                                                                      authorization_data.idTagInfo.value());
            } else {
                this->delete_local_authorization_list_entry(authorization_data.idTag.get().c_str());
            }
        } catch (const QueryExecutionException& e) {
            // catch but continue with remaining entries
            success = false;
        }
    }

    if (!success) {
        throw QueryExecutionException("At least one insertion or deletion of local authorization list entries failed");
    }
}

void DatabaseHandler::delete_local_authorization_list_entry(const std::string& id_tag) {
    std::string sql = "DELETE FROM AUTH_LIST WHERE ID_TAG = @id_tag;";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@id_tag", id_tag);
    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

std::optional<v16::IdTagInfo> DatabaseHandler::get_local_authorization_list_entry(const CiString<20>& id_tag) {
    std::string sql = "SELECT ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG FROM AUTH_LIST WHERE ID_TAG = @id_tag";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@id_tag", id_tag.get(), SQLiteString::Transient);

    int status = stmt->step();

    // entry was not found
    if (status == SQLITE_DONE) {
        return std::nullopt;
    }

    // entry was found
    if (status == SQLITE_ROW) {
        v16::IdTagInfo id_tag_info;
        id_tag_info.status = v16::conversions::string_to_authorization_status(stmt->column_text(1));

        if (stmt->column_type(2) != SQLITE_NULL) {
            id_tag_info.expiryDate.emplace(stmt->column_text(2));
        }

        if (stmt->column_type(3) != SQLITE_NULL) {
            id_tag_info.parentIdTag.emplace(stmt->column_text(3));
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

    throw QueryExecutionException(this->database->get_error_message());
}

void DatabaseHandler::clear_local_authorization_list() {
    const auto retval = this->database->clear_table("AUTH_LIST");
    if (retval == false) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::insert_or_update_charging_profile(const int connector_id, const v16::ChargingProfile& profile) {
    // add or replace
    std::string sql = "INSERT OR REPLACE INTO CHARGING_PROFILES (ID, CONNECTOR_ID, PROFILE) VALUES "
                      "(@id, @connector_id, @profile)";
    auto stmt = this->database->new_statement(sql);

    json json_profile(profile);

    stmt->bind_int("@id", profile.chargingProfileId);
    stmt->bind_int("@connector_id", connector_id);
    stmt->bind_text("@profile", json_profile.dump(), SQLiteString::Transient);

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::delete_charging_profile(const int profile_id) {
    std::string sql = "DELETE FROM CHARGING_PROFILES WHERE ID = @id;";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@id", profile_id);
    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

void DatabaseHandler::delete_charging_profiles() {
    const auto retval = this->database->clear_table("CHARGING_PROFILES");
    if (retval == false) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

std::vector<v16::ChargingProfile> DatabaseHandler::get_charging_profiles() {

    std::vector<v16::ChargingProfile> profiles;
    std::string sql = "SELECT * FROM CHARGING_PROFILES";
    auto stmt = this->database->new_statement(sql);

    int status;
    while ((status = stmt->step()) == SQLITE_ROW) {
        profiles.emplace_back(json::parse(stmt->column_text(2)));
    }

    if (status != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }

    return profiles;
}

int DatabaseHandler::get_connector_id(const int profile_id) {
    std::string sql = "SELECT CONNECTOR_ID FROM CHARGING_PROFILES WHERE ID = @profile_id";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@profile_id", profile_id);

    int status = stmt->step();

    if (status == SQLITE_DONE) {
        throw RequiredEntryNotFoundException("Connector id not found based on charging profile id");
    }

    if (status != SQLITE_ROW) {
        throw QueryExecutionException(this->database->get_error_message());
    }

    return stmt->column_int(0);
}

void DatabaseHandler::insert_ocsp_update() {
    std::string sql = "INSERT OR REPLACE INTO OCSP_REQUEST (LAST_UPDATE) VALUES "
                      "(@last_update)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@last_update", DateTime().to_rfc3339(), SQLiteString::Transient);

    if (stmt->step() != SQLITE_DONE) {
        throw QueryExecutionException(this->database->get_error_message());
    }
}

std::optional<DateTime> DatabaseHandler::get_last_ocsp_update() {
    std::string sql = "SELECT LAST_UPDATE FROM OCSP_REQUEST";
    auto stmt = this->database->new_statement(sql);

    int status = stmt->step();

    if (status == SQLITE_DONE) {
        return std::nullopt;
    }

    if (status == SQLITE_ROW) {
        return DateTime(stmt->column_text(0));
    }

    throw QueryExecutionException(this->database->get_error_message());
}

} // namespace v16
} // namespace ocpp
