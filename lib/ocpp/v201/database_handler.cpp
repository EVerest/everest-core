// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/message_queue.hpp>
#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/types.hpp>
#include <ocpp/v201/utils.hpp>

namespace ocpp {

using namespace common;

namespace v201 {

DatabaseHandler::DatabaseHandler(std::unique_ptr<DatabaseConnectionInterface> database,
                                 const fs::path& sql_migration_files_path) :
    DatabaseHandlerCommon(std::move(database), sql_migration_files_path, MIGRATION_FILE_VERSION_V201) {
}

void DatabaseHandler::init_sql() {
    this->inintialize_enum_tables();
}

void DatabaseHandler::inintialize_enum_tables() {

    // TODO: Don't throw away all meter value items to allow resuming transactions
    // Also we should add functionality then to clean up old/unknown transactions from the database
    if (!this->database->clear_table("METER_VALUE_ITEMS") or !this->database->clear_table("METER_VALUES")) {
        EVLOG_error << "Could not clear tables: " << this->database->get_error_message();
        throw std::runtime_error("Could not clear tables");
    }

    init_enum_table<ReadingContextEnum>("READING_CONTEXT_ENUM", ReadingContextEnum::Interruption_Begin,
                                        ReadingContextEnum::Trigger, conversions::reading_context_enum_to_string);

    init_enum_table<MeasurandEnum>("MEASURAND_ENUM", MeasurandEnum::Current_Export, MeasurandEnum::Voltage,
                                   conversions::measurand_enum_to_string);

    init_enum_table<PhaseEnum>("PHASE_ENUM", PhaseEnum::L1, PhaseEnum::L3_L1, conversions::phase_enum_to_string);

    init_enum_table<LocationEnum>("LOCATION_ENUM", LocationEnum::Body, LocationEnum::Outlet,
                                  conversions::location_enum_to_string);
}

void DatabaseHandler::init_enum_table_inner(const std::string& table_name, const int begin, const int end,
                                            std::function<std::string(int)> conversion) {
    char* err_msg = 0;

    if (!this->database->clear_table(table_name)) {
        EVLOG_critical << "Table \"" + table_name + "\" does not exist";
        throw std::runtime_error("Table does not exist.");
    }

    auto transaction = this->database->begin_transaction();

    std::string sql = "INSERT INTO " + table_name + " VALUES (@id, @value);";
    auto insert_stmt = this->database->new_statement(sql);

    for (int i = begin; i <= end; i++) {
        auto string = conversion(i);

        insert_stmt->bind_int("@id", i);
        insert_stmt->bind_text("@value", string);

        if (insert_stmt->step() != SQLITE_DONE) {
            throw std::runtime_error("Could not perform step.");
        }

        insert_stmt->reset();
    }

    transaction->commit();
}

template <typename T>
void DatabaseHandler::init_enum_table(const std::string& table_name, T begin, T end,
                                      std::function<std::string(T)> conversion) {
    auto conversion_func = [conversion](int value) { return conversion(static_cast<T>(value)); };
    init_enum_table_inner(table_name, static_cast<int>(begin), static_cast<int>(end), conversion_func);
}

void DatabaseHandler::authorization_cache_insert_entry(const std::string& id_token_hash,
                                                       const IdTokenInfo& id_token_info) {
    std::string sql = "INSERT OR REPLACE INTO AUTH_CACHE (ID_TOKEN_HASH, ID_TOKEN_INFO) VALUES "
                      "(@id_token_hash, @id_token_info)";
    auto insert_stmt = this->database->new_statement(sql);

    insert_stmt->bind_text("@id_token_hash", id_token_hash);
    insert_stmt->bind_text("@id_token_info", json(id_token_info).dump(), SQLiteString::Transient);

    if (insert_stmt->step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into AUTH_CACHE table: " << this->database->get_error_message();
        return;
    }
}

std::optional<IdTokenInfo> DatabaseHandler::authorization_cache_get_entry(const std::string& id_token_hash) {
    try {
        std::string sql = "SELECT ID_TOKEN_INFO FROM AUTH_CACHE WHERE ID_TOKEN_HASH = @id_token_hash";
        auto select_stmt = this->database->new_statement(sql);

        select_stmt->bind_text("@id_token_hash", id_token_hash);

        if (select_stmt->step() != SQLITE_ROW) {
            return std::nullopt;
        }
        return IdTokenInfo(json::parse(select_stmt->column_text(0)));
    } catch (const json::exception& e) {
        EVLOG_warning << "Could not parse data of IdTokenInfo: " << e.what();
        return std::nullopt;
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown Error while parsing IdTokenInfo: " << e.what();
        return std::nullopt;
    }
}

void DatabaseHandler::authorization_cache_delete_entry(const std::string& id_token_hash) {
    try {
        std::string sql = "DELETE FROM AUTH_CACHE WHERE ID_TOKEN_HASH = @id_token_hash";
        auto delete_stmt = this->database->new_statement(sql);

        delete_stmt->bind_text("@id_token_hash", id_token_hash);

        if (delete_stmt->step() != SQLITE_DONE) {
            EVLOG_error << "Could not delete from table: " << this->database->get_error_message();
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Exception while deleting from auth cache table: " << e.what();
    }
}

bool DatabaseHandler::authorization_cache_clear() {
    return this->database->clear_table("AUTH_CACHE");
}

size_t DatabaseHandler::authorization_cache_get_binary_size() {
    try {
        std::string sql = "SELECT SUM(\"payload\") FROM \"dbstat\" WHERE name='AUTH_CACHE';";
        auto stmt = this->database->new_statement(sql);

        if (stmt->step() != SQLITE_ROW) {
            throw std::runtime_error("Could not get authorization cache binary size from database");
        }

        return stmt->column_int(0);
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not get authorization cache binary size from database");
    }
}

void DatabaseHandler::insert_availability(int32_t evse_id, int32_t connector_id,
                                          OperationalStatusEnum operational_status, bool replace) {
    std::string sql;

    if (replace) {
        sql = "INSERT OR REPLACE INTO AVAILABILITY (EVSE_ID, CONNECTOR_ID, OPERATIONAL_STATUS) VALUES "
              "(@evse_id, @connector_id, @operational_status)";
    } else {
        sql = "INSERT OR IGNORE INTO AVAILABILITY (EVSE_ID, CONNECTOR_ID, OPERATIONAL_STATUS) VALUES "
              "(@evse_id, @connector_id, @operational_status)";
    }

    auto insert_stmt = this->database->new_statement(sql);

    insert_stmt->bind_int("@evse_id", evse_id);
    insert_stmt->bind_int("@connector_id", connector_id);

    insert_stmt->bind_text("@operational_status", conversions::operational_status_enum_to_string(operational_status),
                           SQLiteString::Transient);

    if (insert_stmt->step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into AVAILABILITY table: " << this->database->get_error_message();
        return;
    }
}

OperationalStatusEnum DatabaseHandler::get_availability(int32_t evse_id, int32_t connector_id) {
    try {
        std::string sql =
            "SELECT OPERATIONAL_STATUS FROM AVAILABILITY WHERE EVSE_ID = @evse_id AND CONNECTOR_ID = @connector_id;";
        auto select_stmt = this->database->new_statement(sql);

        select_stmt->bind_int("@evse_id", evse_id);
        select_stmt->bind_int("@connector_id", connector_id);

        if (select_stmt->step() != SQLITE_ROW) {
            throw std::runtime_error("Could not get availability from database");
        }
        return conversions::string_to_operational_status_enum(select_stmt->column_text(0));
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not get availability from database");
    }
}

void DatabaseHandler::insert_or_update_local_authorization_list_version(int32_t version) {
    std::string sql = "INSERT OR REPLACE INTO AUTH_LIST_VERSION (ID, VERSION) VALUES (0, @version)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_int("@version", version);

    if (stmt->step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << this->database->get_error_message();
        throw std::runtime_error("db access error");
    }
}

int32_t DatabaseHandler::get_local_authorization_list_version() {
    std::string sql = "SELECT VERSION FROM AUTH_LIST_VERSION WHERE ID = 0";
    auto stmt = this->database->new_statement(sql);

    if (stmt->step() != SQLITE_ROW) {
        EVLOG_error << "Error selecting auth list version: " << this->database->get_error_message();
        throw std::runtime_error("db access error");
    }

    return stmt->column_int(0);
}

void DatabaseHandler::insert_or_update_local_authorization_list_entry(const IdToken& id_token,
                                                                      const IdTokenInfo& id_token_info) {
    // add or replace
    std::string sql = "INSERT OR REPLACE INTO AUTH_LIST (ID_TOKEN_HASH, ID_TOKEN_INFO) "
                      "VALUES (@id_token_hash, @id_token_info)";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@id_token_hash", utils::generate_token_hash(id_token), SQLiteString::Transient);
    stmt->bind_text("@id_token_info", json(id_token_info).dump(), SQLiteString::Transient);

    if (stmt->step() != SQLITE_DONE) {
        EVLOG_error << "Could not insert into table: " << this->database->get_error_message();
        throw std::runtime_error("db access error");
    }
}

void DatabaseHandler::insert_or_update_local_authorization_list(
    const std::vector<AuthorizationData>& local_authorization_list) {
    for (const auto& authorization_data : local_authorization_list) {
        if (authorization_data.idTokenInfo.has_value()) {
            this->insert_or_update_local_authorization_list_entry(authorization_data.idToken,
                                                                  authorization_data.idTokenInfo.value());
        } else {
            this->delete_local_authorization_list_entry(authorization_data.idToken);
        }
    }
}

void DatabaseHandler::delete_local_authorization_list_entry(const IdToken& id_token) {
    std::string sql = "DELETE FROM AUTH_LIST WHERE ID_TOKEN_HASH = @id_token_hash;";
    auto stmt = this->database->new_statement(sql);

    stmt->bind_text("@id_token_hash", utils::generate_token_hash(id_token), SQLiteString::Transient);

    if (stmt->step() != SQLITE_DONE) {
        EVLOG_error << "Could not delete from table: " << this->database->get_error_message();
    }
}

std::optional<IdTokenInfo> DatabaseHandler::get_local_authorization_list_entry(const IdToken& id_token) {
    try {
        std::string sql = "SELECT ID_TOKEN_INFO FROM AUTH_LIST WHERE ID_TOKEN_HASH = @id_token_hash;";
        auto stmt = this->database->new_statement(sql);

        stmt->bind_text("@id_token_hash", utils::generate_token_hash(id_token), SQLiteString::Transient);

        if (stmt->step() != SQLITE_ROW) {
            return std::nullopt;
        }
        return IdTokenInfo(json::parse(stmt->column_text(0)));
    } catch (const json::exception& e) {
        EVLOG_warning << "Could not parse data of IdTokenInfo: " << e.what();
        return std::nullopt;
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown Error while parsing IdTokenInfo: " << e.what();
        return std::nullopt;
    }
}

bool DatabaseHandler::clear_local_authorization_list() {
    return this->database->clear_table("AUTH_LIST");
}

int32_t DatabaseHandler::get_local_authorization_list_number_of_entries() {
    try {
        std::string sql = "SELECT COUNT(*) FROM AUTH_LIST;";
        auto stmt = this->database->new_statement(sql);

        if (stmt->step() != SQLITE_ROW) {
            throw std::runtime_error("Could not get local list count from database");
        }

        return stmt->column_int(0);
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not get local list count from database");
    }
}

bool DatabaseHandler::transaction_metervalues_insert(const std::string& transaction_id, const MeterValue& meter_value) {
    if (meter_value.sampledValue.empty()) {
        return false;
    }

    auto sampled_value_context = meter_value.sampledValue.at(0).context;
    if (!sampled_value_context.has_value()) {
        return false;
    }

    auto context = sampled_value_context.value();
    if (std::find_if(meter_value.sampledValue.begin(), meter_value.sampledValue.end(), [context](const auto& item) {
            return !item.context.has_value() or item.context.value() != context;
        }) != meter_value.sampledValue.end()) {
        throw std::invalid_argument("All metervalues must have the same context");
    }

    char* err_msg = 0;

    std::string sql1 = "INSERT INTO METER_VALUES (TRANSACTION_ID, TIMESTAMP, READING_CONTEXT, CUSTOM_DATA) VALUES "
                       "(@transaction_id, @timestamp, @context, @custom_data)";

    auto stmt = this->database->new_statement(sql1);

    stmt->bind_text("@transaction_id", transaction_id);
    stmt->bind_datetime("@timestamp", meter_value.timestamp);
    stmt->bind_int("@context", static_cast<int>(context));
    stmt->bind_null("@custom_data");

    if (stmt->step() != SQLITE_DONE) {
        EVLOG_critical << "Error: " << this->database->get_error_message();
        throw std::runtime_error("Could not perform step.");
    }

    auto last_row_id = this->database->get_last_inserted_rowid();
    stmt->reset();

    std::string sql2 = "INSERT INTO METER_VALUE_ITEMS (METER_VALUE_ID, VALUE, MEASURAND, PHASE, LOCATION, CUSTOM_DATA, "
                       "UNIT_CUSTOM_DATA, UNIT_TEXT, UNIT_MULTIPLIER, SIGNED_METER_DATA, SIGNING_METHOD, "
                       "ENCODING_METHOD, PUBLIC_KEY) VALUES (@meter_value_id, @value, @measurand, "
                       "@phase, @location, @custom_data, @unit_custom_data, @unit_text, @unit_multiplier, "
                       "@signed_meter_data, @signing_method, @encoding_method, @public_key);";

    auto transaction = this->database->begin_transaction();
    auto insert_stmt = this->database->new_statement(sql2);

    for (const auto& item : meter_value.sampledValue) {
        insert_stmt->bind_int("@meter_value_id", last_row_id);
        insert_stmt->bind_double("@value", item.value);

        if (item.measurand.has_value()) {
            insert_stmt->bind_int("@measurand", static_cast<int>(item.measurand.value()));
        } else {
            insert_stmt->bind_null("@measurand");
        }

        if (item.phase.has_value()) {
            insert_stmt->bind_int("@phase", static_cast<int>(item.phase.value()));
        } else {
            insert_stmt->bind_null("@phase");
        }

        if (item.location.has_value()) {
            insert_stmt->bind_int("@location", static_cast<int>(item.location.value()));
        }

        if (item.customData.has_value()) {
            insert_stmt->bind_text("@custom_data", item.customData.value().at("vendorId").get<std::string>(),
                                   SQLiteString::Transient);
        }

        if (item.unitOfMeasure.has_value()) {
            const auto& unitOfMeasure = item.unitOfMeasure.value();

            if (unitOfMeasure.customData.has_value()) {
                insert_stmt->bind_text("@unit_custom_data",
                                       unitOfMeasure.customData.value().at("vendorId").get<std::string>(),
                                       SQLiteString::Transient);
            }
            if (unitOfMeasure.unit.has_value()) {
                insert_stmt->bind_text("@unit_text", unitOfMeasure.unit.value().get(), SQLiteString::Transient);
            }
            if (unitOfMeasure.multiplier.has_value()) {
                insert_stmt->bind_int("@unit_multiplier", unitOfMeasure.multiplier.value());
            }
        }

        if (item.signedMeterValue.has_value()) {
            const auto& signedMeterValue = item.signedMeterValue.value();

            insert_stmt->bind_text("@signed_meter_data", signedMeterValue.signedMeterData.get(),
                                   SQLiteString::Transient);
            insert_stmt->bind_text("@signing_method", signedMeterValue.signingMethod.get(), SQLiteString::Transient);
            insert_stmt->bind_text("@encoding_method", signedMeterValue.encodingMethod.get(), SQLiteString::Transient);
            insert_stmt->bind_text("@public_key", signedMeterValue.publicKey.get(), SQLiteString::Transient);
        } else {
            insert_stmt->bind_null("@signed_meter_data");
            insert_stmt->bind_null("@signing_method");
            insert_stmt->bind_null("@encoding_method");
            insert_stmt->bind_null("@public_key");
        }

        if (insert_stmt->step() != SQLITE_DONE) {
            EVLOG_critical << "Error: " << this->database->get_error_message();
            throw std::runtime_error("Could not perform step.");
        }

        insert_stmt->reset();
    }

    transaction->commit();

    return true;
}

std::vector<MeterValue> DatabaseHandler::transaction_metervalues_get_all(const std::string& transaction_id) {

    std::string sql1 = "SELECT * FROM METER_VALUES WHERE TRANSACTION_ID = @transaction_id;";
    std::string sql2 = "SELECT * FROM METER_VALUE_ITEMS WHERE METER_VALUE_ID = @row_id;";
    auto select_stmt = this->database->new_statement(sql1);
    auto select_stmt2 = this->database->new_statement(sql2);

    select_stmt->bind_text("@transaction_id", transaction_id);

    std::vector<MeterValue> result;

    while (select_stmt->step() == SQLITE_ROW) {
        MeterValue value;

        value.timestamp = select_stmt->column_datetime(2);

        if (select_stmt->column_type(4) == SQLITE_TEXT) {
            value.customData = CustomData{select_stmt->column_text(4)};
        }

        auto row_id = select_stmt->column_int(0);
        auto context = static_cast<ReadingContextEnum>(select_stmt->column_int(3));

        select_stmt2->bind_int("@row_id", row_id);

        while (select_stmt2->step() == SQLITE_ROW) {
            SampledValue sampled_value;

            sampled_value.value = select_stmt2->column_double(1);
            sampled_value.context = context;

            if (select_stmt2->column_type(2) == SQLITE_INTEGER) {
                sampled_value.measurand = static_cast<MeasurandEnum>(select_stmt2->column_int(2));
            }

            if (select_stmt2->column_type(3) == SQLITE_INTEGER) {
                sampled_value.phase = static_cast<PhaseEnum>(select_stmt2->column_int(3));
            }

            if (select_stmt2->column_type(4) == SQLITE_INTEGER) {
                sampled_value.location = static_cast<LocationEnum>(select_stmt2->column_int(4));
            }

            if (select_stmt2->column_type(5) == SQLITE_TEXT) {
                sampled_value.customData = CustomData{select_stmt->column_text(5)};
            }

            if (select_stmt2->column_type(6) == SQLITE_TEXT or select_stmt2->column_type(7) == SQLITE_TEXT or
                select_stmt2->column_type(8) == SQLITE_INTEGER) {
                UnitOfMeasure unit;
                if (select_stmt2->column_type(6) == SQLITE_TEXT) {
                    unit.customData = CustomData{select_stmt2->column_text(6)};
                }
                if (select_stmt2->column_type(7) == SQLITE_TEXT) {
                    unit.unit = select_stmt2->column_text(7);
                }
                if (select_stmt2->column_type(8) == SQLITE_INTEGER) {
                    unit.multiplier = select_stmt2->column_int(8);
                }
                sampled_value.unitOfMeasure.emplace(unit);
            }

            if (select_stmt2->column_type(9) == SQLITE_TEXT and select_stmt2->column_type(10) == SQLITE_TEXT and
                select_stmt2->column_type(11) == SQLITE_TEXT and select_stmt2->column_type(12) == SQLITE_TEXT) {
                SignedMeterValue signed_meter_value;
                signed_meter_value.signedMeterData = select_stmt2->column_text(9);
                signed_meter_value.signingMethod = select_stmt2->column_text(10);
                signed_meter_value.encodingMethod = select_stmt2->column_text(11);
                signed_meter_value.publicKey = select_stmt2->column_text(12);

                sampled_value.signedMeterValue.emplace(signed_meter_value);
            }

            value.sampledValue.push_back(std::move(sampled_value));
        }

        result.push_back(std::move(value));

        select_stmt2->reset();
    }

    return result;
}

bool DatabaseHandler::transaction_metervalues_clear(const std::string& transaction_id) {

    std::string sql1 = "SELECT ROWID FROM METER_VALUES WHERE TRANSACTION_ID = @transaction_id;";

    auto select_stmt = this->database->new_statement(sql1);

    select_stmt->bind_text("@transaction_id", transaction_id);

    std::string sql2 = "DELETE FROM METER_VALUE_ITEMS WHERE METER_VALUE_ID = @row_id";
    auto delete_stmt = this->database->new_statement(sql2);
    while (select_stmt->step() == SQLITE_ROW) {
        auto row_id = select_stmt->column_int(0);
        delete_stmt->bind_int("@row_id", row_id);

        if (delete_stmt->step() != SQLITE_DONE) {
            EVLOG_error << "Could not delete from table: " << this->database->get_error_message();
        }
        delete_stmt->reset();
    }

    std::string sql3 = "DELETE FROM METER_VALUES WHERE TRANSACTION_ID = @transaction_id";
    auto delete_stmt2 = this->database->new_statement(sql3);
    delete_stmt2->bind_text("@transaction_id", transaction_id);
    if (delete_stmt2->step() != SQLITE_DONE) {
        EVLOG_error << "Could not delete from table: " << this->database->get_error_message();
    }

    return true;
}

void DatabaseHandler::insert_cs_availability(OperationalStatusEnum operational_status, bool replace) {
    this->insert_availability(0, 0, operational_status, replace);
}

OperationalStatusEnum DatabaseHandler::get_cs_availability() {
    return this->get_availability(0, 0);
}

void DatabaseHandler::insert_evse_availability(int32_t evse_id, OperationalStatusEnum operational_status,
                                               bool replace) {
    assert(evse_id > 0);
    this->insert_availability(evse_id, 0, operational_status, replace);
}

OperationalStatusEnum DatabaseHandler::get_evse_availability(int32_t evse_id) {
    assert(evse_id > 0);
    return this->get_availability(evse_id, 0);
}

void DatabaseHandler::insert_connector_availability(int32_t evse_id, int32_t connector_id,
                                                    OperationalStatusEnum operational_status, bool replace) {
    assert(evse_id > 0);
    assert(connector_id > 0);
    this->insert_availability(evse_id, connector_id, operational_status, replace);
}

OperationalStatusEnum DatabaseHandler::get_connector_availability(int32_t evse_id, int32_t connector_id) {
    assert(evse_id > 0);
    assert(connector_id > 0);
    return this->get_availability(evse_id, connector_id);
}

} // namespace v201
} // namespace ocpp
