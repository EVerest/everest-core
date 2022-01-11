// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <boost/algorithm/string/join.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/schemas.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {
ChargePointConfiguration::ChargePointConfiguration(json config, std::string schemas_path, std::string database_path) {
    // validate config entries
    Schemas schemas = Schemas(schemas_path);
    auto patch = schemas.get_profile_validator()->validate(config);
    if (patch.is_null()) {
        // no defaults substituted
        EVLOG(debug) << "Using a charge point configuration without default values.";
        this->config = config;
    } else {
        // extend config with default values
        EVLOG(debug) << "Adding the following default values to the charge point configuration: " << patch;
        auto patched_config = config.patch(patch);
        this->config = patched_config;
    }

    if (!this->config["Core"].contains("SupportedFeatureProfiles")) {
        throw std::runtime_error("SupportedFeatureProfiles key is missing from config");
    }

    {
        std::vector<std::string> components;
        auto profiles = this->getSupportedFeatureProfiles();
        if (!profiles.empty()) {
            boost::split(components, profiles, boost::is_any_of(","));
            for (auto component : components) {
                try {
                    this->supported_feature_profiles.insert(
                        conversions::string_to_supported_feature_profiles(component));
                } catch (const std::out_of_range& e) {
                    EVLOG(error) << "Feature profile: \"" << component << "\" not recognized";
                    throw std::runtime_error("Unknown component in SupportedFeatureProfiles config option.");
                }
            }
        }
    }
    if (this->supported_feature_profiles.count(SupportedFeatureProfiles::Core) == 0) {
        throw std::runtime_error("Core profile not listed in SupportedFeatureProfiles. This is required.");
    }

    // open and initialize database
    std::string sqlite_db_filename = this->config["Internal"]["ChargePointId"].get<std::string>() + ".db";
    boost::filesystem::path database_directory = boost::filesystem::path(database_path);
    if (!boost::filesystem::exists(database_directory)) {
        boost::filesystem::create_directories(database_directory);
    }
    boost::filesystem::path sqlite_db_path = database_directory / sqlite_db_filename;

    int ret = sqlite3_open(sqlite_db_path.c_str(), &this->db);

    if (ret != SQLITE_OK) {
        EVLOG(error) << "Error opening database '" << sqlite_db_path << "': " << sqlite3_errmsg(db);
        throw std::runtime_error("Could not open database at provided path.");
    }

    EVLOG(debug) << "sqlite version: " << sqlite3_libversion();

    // prepare the database
    std::string create_sql = "CREATE TABLE IF NOT EXISTS CONNECTORS ("
                             "ID INT PRIMARY KEY     NOT NULL,"
                             "AVAILABILITY           TEXT);";

    sqlite3_stmt* create_statement;
    sqlite3_prepare_v2(this->db, create_sql.c_str(), create_sql.size(), &create_statement, NULL);
    int res = sqlite3_step(create_statement);
    if (res != SQLITE_DONE) {
        EVLOG(error) << "Could not create table: " << res << sqlite3_errmsg(this->db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(create_statement) != SQLITE_OK) {
        EVLOG(error) << "Error creating table";
        throw std::runtime_error("db access error");
    }

    for (int32_t i = 1; i <= this->getNumberOfConnectors(); i++) {
        std::ostringstream insert_sql;
        insert_sql << "INSERT OR IGNORE INTO CONNECTORS (ID, AVAILABILITY) VALUES (" << i << ", \""
                   << conversions::availability_type_to_string(AvailabilityType::Operative) << "\")";
        std::string insert_sql_str = insert_sql.str();
        sqlite3_stmt* insert_statement;
        sqlite3_prepare_v2(this->db, insert_sql_str.c_str(), insert_sql_str.size(), &insert_statement, NULL);
        int res = sqlite3_step(insert_statement);
        if (res != SQLITE_DONE) {
            EVLOG(error) << "Could not insert into table: " << res << sqlite3_errmsg(this->db);
            throw std::runtime_error("db access error");
        }

        if (sqlite3_finalize(insert_statement) != SQLITE_OK) {
            EVLOG(error) << "Error inserting into table";
            throw std::runtime_error("db access error");
        }
    }

    // prepare authorization cache
    std::string create_auth_cache_sql = "CREATE TABLE IF NOT EXISTS AUTH_CACHE ("
                                        "ID_TAG TEXT PRIMARY KEY     NOT NULL,"
                                        "AUTH_STATUS TEXT NOT NULL,"
                                        "EXPIRY_DATE TEXT,"
                                        "PARENT_ID_TAG TEXT);";

    sqlite3_stmt* create_auth_cache_statement;
    sqlite3_prepare_v2(this->db, create_auth_cache_sql.c_str(), create_auth_cache_sql.size(),
                       &create_auth_cache_statement, NULL);
    int create_auth_cache_res = sqlite3_step(create_auth_cache_statement);
    if (create_auth_cache_res != SQLITE_DONE) {
        EVLOG(error) << "Could not create table AUTH_CACHE: " << create_auth_cache_res << sqlite3_errmsg(this->db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(create_auth_cache_statement) != SQLITE_OK) {
        EVLOG(error) << "Error creating table AUTH_CACHE";
        throw std::runtime_error("db access error");
    }

    // TODO(kai): get this from config
    this->supported_measurands = {{Measurand::Energy_Active_Import_Register, {Phase::L1, Phase::L2, Phase::L3}}, // Wh
                                  {Measurand::Energy_Active_Export_Register, {Phase::L1, Phase::L2, Phase::L3}}, // Wh
                                  {Measurand::Power_Active_Import, {Phase::L1, Phase::L2, Phase::L3}},           // W
                                  {Measurand::Voltage, {Phase::L1, Phase::L2, Phase::L3}},                       // V
                                  {Measurand::Current_Import, {Phase::L1, Phase::L2, Phase::L3, Phase::N}},      // A
                                  {Measurand::Frequency, {Phase::L1, Phase::L2, Phase::L3}},                     // Hz
                                  {Measurand::Current_Offered, {}}};                                             // A

    this->supported_message_types_from_charge_point = {
        {SupportedFeatureProfiles::Core,
         {MessageType::Authorize, MessageType::BootNotification, MessageType::ChangeAvailabilityResponse,
          MessageType::ChangeConfigurationResponse, MessageType::ClearCacheResponse, MessageType::DataTransfer,
          MessageType::DataTransferResponse, MessageType::GetConfigurationResponse, MessageType::Heartbeat,
          MessageType::MeterValues, MessageType::RemoteStartTransactionResponse,
          MessageType::RemoteStopTransactionResponse, MessageType::ResetResponse, MessageType::StartTransaction,
          MessageType::StatusNotification, MessageType::StopTransaction, MessageType::UnlockConnectorResponse}},
        {SupportedFeatureProfiles::FirmwareManagement,
         {MessageType::GetDiagnosticsResponse, MessageType::DiagnosticsStatusNotification,
          MessageType::FirmwareStatusNotification, MessageType::UpdateFirmwareResponse}},
        {SupportedFeatureProfiles::LocalAuthListManagement,
         {MessageType::GetLocalListVersionResponse, MessageType::SendLocalListResponse}},
        {SupportedFeatureProfiles::RemoteTrigger, {MessageType::TriggerMessageResponse}},
        {SupportedFeatureProfiles::Reservation,
         {MessageType::CancelReservationResponse, MessageType::ReserveNowResponse}},
        {SupportedFeatureProfiles::SmartCharging,
         {MessageType::ClearChargingProfileResponse, MessageType::GetCompositeScheduleResponse,
          MessageType::SetChargingProfileResponse}}};

    this->supported_message_types_from_central_system = {
        {SupportedFeatureProfiles::Core,
         {MessageType::AuthorizeResponse, MessageType::BootNotificationResponse, MessageType::ChangeAvailability,
          MessageType::ChangeConfiguration, MessageType::ClearCache, MessageType::DataTransfer,
          MessageType::DataTransferResponse, MessageType::GetConfiguration, MessageType::HeartbeatResponse,
          MessageType::MeterValuesResponse, MessageType::RemoteStartTransaction, MessageType::RemoteStopTransaction,
          MessageType::Reset, MessageType::StartTransactionResponse, MessageType::StatusNotificationResponse,
          MessageType::StopTransactionResponse, MessageType::UnlockConnector}},
        {SupportedFeatureProfiles::FirmwareManagement,
         {MessageType::GetDiagnostics, MessageType::DiagnosticsStatusNotificationResponse,
          MessageType::FirmwareStatusNotificationResponse, MessageType::UpdateFirmware}},
        {SupportedFeatureProfiles::LocalAuthListManagement,
         {MessageType::GetLocalListVersion, MessageType::SendLocalList}},
        {SupportedFeatureProfiles::RemoteTrigger, {MessageType::TriggerMessage}},
        {SupportedFeatureProfiles::Reservation, {MessageType::CancelReservation, MessageType::ReserveNow}},
        {SupportedFeatureProfiles::SmartCharging,
         {MessageType::ClearChargingProfile, MessageType::GetCompositeSchedule, MessageType::SetChargingProfile}}};

    for (auto feature_profile : this->supported_feature_profiles) {
        this->supported_message_types_sending.insert(
            this->supported_message_types_from_charge_point[feature_profile].begin(),
            this->supported_message_types_from_charge_point[feature_profile].end());

        this->supported_message_types_receiving.insert(
            this->supported_message_types_from_central_system[feature_profile].begin(),
            this->supported_message_types_from_central_system[feature_profile].end());
    }
}

void ChargePointConfiguration::close() {
    EVLOG(info) << "Closing database file...";
    int ret = sqlite3_close(this->db);
    if (ret == SQLITE_OK) {
        EVLOG(info) << "Successfully closed database file";
    } else {
        EVLOG(error) << "Error closing database file: " << ret;
    }
}

// Internal config options
std::string ChargePointConfiguration::getChargePointId() {
    return this->config["Internal"]["ChargePointId"];
}
std::string ChargePointConfiguration::getCentralSystemURI() {
    return this->config["Internal"]["CentralSystemURI"];
}
boost::optional<std::string> ChargePointConfiguration::getChargeBoxSerialNumber() {
    boost::optional<std::string> charge_box_serial_number = boost::none;
    if (this->config["Internal"].contains("ChargeBoxSerialNumber")) {
        charge_box_serial_number.emplace(this->config["Internal"]["ChargeBoxSerialNumber"]);
    }
    return charge_box_serial_number;
}
std::string ChargePointConfiguration::getChargePointModel() {
    return this->config["Internal"]["ChargePointModel"];
}
boost::optional<std::string> ChargePointConfiguration::getChargePointSerialNumber() {
    boost::optional<std::string> charge_point_serial_number = boost::none;
    if (this->config["Internal"].contains("ChargePointSerialNumber")) {
        charge_point_serial_number.emplace(this->config["Internal"]["ChargePointSerialNumber"]);
    }
    return charge_point_serial_number;
}
std::string ChargePointConfiguration::getChargePointVendor() {
    return this->config["Internal"]["ChargePointVendor"];
}
std::string ChargePointConfiguration::getFirmwareVersion() {
    return this->config["Internal"]["FirmwareVersion"];
}
boost::optional<std::string> ChargePointConfiguration::getICCID() {
    boost::optional<std::string> iccid = boost::none;
    if (this->config["Internal"].contains("ICCID")) {
        iccid.emplace(this->config["Internal"]["ICCID"]);
    }
    return iccid;
}
boost::optional<std::string> ChargePointConfiguration::getIMSI() {
    boost::optional<std::string> imsi = boost::none;
    if (this->config["Internal"].contains("IMSI")) {
        imsi.emplace(this->config["Internal"]["IMSI"]);
    }
    return imsi;
}
boost::optional<std::string> ChargePointConfiguration::getMeterSerialNumber() {
    boost::optional<std::string> meter_serial_number = boost::none;
    if (this->config["Internal"].contains("MeterSerialNumber")) {
        meter_serial_number.emplace(this->config["Internal"]["MeterSerialNumber"]);
    }
    return meter_serial_number;
}
boost::optional<std::string> ChargePointConfiguration::getMeterType() {
    boost::optional<std::string> meter_type = boost::none;
    if (this->config["Internal"].contains("MeterType")) {
        meter_type.emplace(this->config["Internal"]["MeterType"]);
    }
    return meter_type;
}
int32_t ChargePointConfiguration::getWebsocketReconnectInterval() {
    return this->config["Internal"]["WebsocketReconnectInterval"];
}
std::string ChargePointConfiguration::getSupportedCiphers() {

    std::vector<std::string> supported_ciphers = this->config["Internal"]["SupportedCiphers"];
    return boost::algorithm::join(supported_ciphers, ":");
}
boost::optional<std::string> ChargePointConfiguration::getAuthorizationKey() {
    boost::optional<std::string> authorization_key = boost::none;
    if (this->config["Internal"].contains("AuthorizationKey")) {
        authorization_key.emplace(this->config["Internal"]["AuthorizationKey"]);
    }
    return authorization_key;
}

std::vector<MeasurandWithPhase> ChargePointConfiguration::csv_to_measurand_with_phase_vector(std::string csv) {
    std::vector<std::string> components;

    boost::split(components, csv, boost::is_any_of(","));
    std::vector<MeasurandWithPhase> measurand_with_phase_vector;
    for (auto component : components) {
        MeasurandWithPhase measurand_with_phase;
        try {
            Measurand measurand = ocpp1_6::conversions::string_to_measurand(component);
            // check if this measurand can be provided on multiple phases
            if (this->supported_measurands[measurand].size() > 0) {
                // multiple phases are available
                // also add the measurand without a phase as a total value
                measurand_with_phase.measurand = measurand;
                measurand_with_phase_vector.push_back(measurand_with_phase);

                for (auto phase : this->supported_measurands[measurand]) {
                    measurand_with_phase.phase.emplace(phase);
                    if (std::find(measurand_with_phase_vector.begin(), measurand_with_phase_vector.end(),
                                  measurand_with_phase) == measurand_with_phase_vector.end()) {
                        measurand_with_phase_vector.push_back(measurand_with_phase);
                    }
                }
            } else {
                // this is a measurand without any phase support
                measurand_with_phase.measurand = measurand;
                if (std::find(measurand_with_phase_vector.begin(), measurand_with_phase_vector.end(),
                              measurand_with_phase) == measurand_with_phase_vector.end()) {
                    measurand_with_phase_vector.push_back(measurand_with_phase);
                }
            }
        } catch (std::out_of_range& o) {
            std::vector<std::string> measurand_with_phase_vec;
            iter_split(measurand_with_phase_vec, component, boost::algorithm::last_finder("."));
            measurand_with_phase.measurand = ocpp1_6::conversions::string_to_measurand(measurand_with_phase_vec.at(0));
            measurand_with_phase.phase.emplace(ocpp1_6::conversions::string_to_phase(measurand_with_phase_vec.at(1)));

            if (std::find(measurand_with_phase_vector.begin(), measurand_with_phase_vector.end(),
                          measurand_with_phase) == measurand_with_phase_vector.end()) {
                measurand_with_phase_vector.push_back(measurand_with_phase);
            }
        }
    }
    for (auto m : measurand_with_phase_vector) {
        if (!m.phase) {
            EVLOG(debug) << "measurand without phase: " << m.measurand;

        } else {
            EVLOG(debug) << "measurand: " << m.measurand
                         << " with phase: " << ocpp1_6::conversions::phase_to_string(m.phase.value());
        }
    }
    return measurand_with_phase_vector;
}

bool ChargePointConfiguration::measurands_supported(std::string csv) {
    auto requested_measurands = this->csv_to_measurand_with_phase_vector(csv);
    // check if the requested measurands are supported, otherwise return false
    for (auto req : requested_measurands) {
        EVLOG(error) << "requested_measurand: " << conversions::measurand_to_string(req.measurand);
        if (this->supported_measurands.count(req.measurand) == 0) {
            return false;
        }

        if (req.phase) {
            auto phase = req.phase.value();
            auto measurand = this->supported_measurands[req.measurand];
            EVLOG(error) << "  there was specific phase requested...: " << conversions::phase_to_string(phase);

            if (std::find(measurand.begin(), measurand.end(), phase) == measurand.end()) {
                // phase not found, this is an error
                return false;
            }
        }
    }
    return true;
}

std::set<MessageType> ChargePointConfiguration::getSupportedMessageTypesSending() {
    return this->supported_message_types_sending;
}

std::set<MessageType> ChargePointConfiguration::getSupportedMessageTypesReceiving() {
    return this->supported_message_types_receiving;
}

bool ChargePointConfiguration::setConnectorAvailability(int32_t connectorId, AvailabilityType availability) {
    int32_t number_of_connectors = this->getNumberOfConnectors();
    if (connectorId > number_of_connectors) {
        EVLOG(warning) << "trying to set the availability of a connector that does not exist: " << connectorId
                       << ", there are only " << number_of_connectors << " connectors.";
        return false;
    }
    std::vector<int32_t> connectors;
    if (connectorId == 0) {
        EVLOG(debug) << "changing availability of all connectors";
        for (int32_t i = 1; i <= number_of_connectors; i++) {
            connectors.push_back(i);
        }
    } else {
        connectors.push_back(connectorId);
    }

    for (auto connector : connectors) {
        std::ostringstream insert_sql;
        insert_sql << "INSERT OR REPLACE INTO CONNECTORS (ID, AVAILABILITY) VALUES (" << connector << ", \""
                   << conversions::availability_type_to_string(availability) << "\")";
        std::string insert_sql_str = insert_sql.str();
        sqlite3_stmt* insert_statement;
        sqlite3_prepare_v2(db, insert_sql_str.c_str(), insert_sql_str.size(), &insert_statement, NULL);
        int res = sqlite3_step(insert_statement);
        if (res != SQLITE_DONE) {
            EVLOG(error) << "Could not insert into table: " << res << sqlite3_errmsg(db);
            throw std::runtime_error("db access error");
        }

        if (sqlite3_finalize(insert_statement) != SQLITE_OK) {
            EVLOG(error) << "Error inserting into table";
            throw std::runtime_error("db access error");
        }
    }

    return true;
}
AvailabilityType ChargePointConfiguration::getConnectorAvailability(int32_t connectorId) {
    std::ostringstream select_sql;
    std::promise<ocpp1_6::AvailabilityType>* sql_promise = new std::promise<ocpp1_6::AvailabilityType>();
    std::future<ocpp1_6::AvailabilityType> sql_future = sql_promise->get_future();

    select_sql << "SELECT AVAILABILITY FROM CONNECTORS WHERE ID = " << connectorId << ";";
    std::string select_sql_str = select_sql.str();
    char* error = nullptr;
    int res = sqlite3_exec(
        db, select_sql_str.c_str(),
        [](void* sql_promise, int count, char** results, char** column_name) -> int {
            if (count == 1) {
                static_cast<std::promise<ocpp1_6::AvailabilityType>*>(sql_promise)
                    ->set_value(ocpp1_6::conversions::string_to_availability_type(results[0]));
            }
            return 0;
        },
        (void*)sql_promise, &error);

    std::chrono::system_clock::time_point sql_wait = std::chrono::system_clock::now() + ocpp1_6::future_wait_seconds;
    std::future_status sql_future_status;
    do {
        sql_future_status = sql_future.wait_until(sql_wait);
    } while (sql_future_status == std::future_status::deferred);
    if (sql_future_status == std::future_status::timeout) {
        EVLOG(debug) << "sql future timeout";
    } else if (sql_future_status == std::future_status::ready) {
        EVLOG(debug) << "sql future ready";
    }

    ocpp1_6::AvailabilityType response = sql_future.get();
    return response;
}

std::map<int32_t, ocpp1_6::AvailabilityType> ChargePointConfiguration::getConnectorAvailability() {
    std::ostringstream select_sql;
    std::promise<std::map<int32_t, ocpp1_6::AvailabilityType>>* sql_promise =
        new std::promise<std::map<int32_t, ocpp1_6::AvailabilityType>>();
    std::future<std::map<int32_t, ocpp1_6::AvailabilityType>> sql_future = sql_promise->get_future();

    select_sql << "SELECT AVAILABILITY FROM CONNECTORS";
    std::string select_sql_str = select_sql.str();
    char* error = nullptr;
    int res = sqlite3_exec(
        db, select_sql_str.c_str(),
        [](void* sql_promise, int count, char** results, char** column_name) -> int {
            std::map<int32_t, ocpp1_6::AvailabilityType> connector_availability;
            for (int connector = 0; connector < count; connector++) {
                connector_availability[connector + 1] =
                    ocpp1_6::conversions::string_to_availability_type(results[connector]);
            }
            static_cast<std::promise<std::map<int32_t, ocpp1_6::AvailabilityType>>*>(sql_promise)
                ->set_value(connector_availability);
            return 0;
        },
        (void*)sql_promise, &error);

    std::chrono::system_clock::time_point sql_wait = std::chrono::system_clock::now() + ocpp1_6::future_wait_seconds;
    std::future_status sql_future_status;
    do {
        sql_future_status = sql_future.wait_until(sql_wait);
    } while (sql_future_status == std::future_status::deferred);
    if (sql_future_status == std::future_status::timeout) {
        EVLOG(debug) << "sql future timeout";
    } else if (sql_future_status == std::future_status::ready) {
        EVLOG(debug) << "sql future ready";
    }

    std::map<int32_t, ocpp1_6::AvailabilityType> response = sql_future.get();
    return response;
}

bool ChargePointConfiguration::updateAuthorizationCacheEntry(CiString20Type idTag, IdTagInfo idTagInfo) {
    if (!this->getAuthorizationCacheEnabled()) {
        return false;
    }
    std::ostringstream insert_sql;
    insert_sql << "INSERT OR REPLACE INTO AUTH_CACHE (ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG) VALUES "
                  "(@id_tag, @auth_status, @expiry_date, @parent_id_tag)";
    std::string insert_sql_str = insert_sql.str();
    sqlite3_stmt* insert_statement;
    sqlite3_prepare_v2(db, insert_sql_str.c_str(), insert_sql_str.size(), &insert_statement, NULL);

    auto idTag_str = idTag.get();
    auto idTagInfo_str = ocpp1_6::conversions::authorization_status_to_string(idTagInfo.status);
    sqlite3_bind_text(insert_statement, 1, idTag_str.c_str(), -1, NULL);
    sqlite3_bind_text(insert_statement, 2, idTagInfo_str.c_str(), -1, NULL);
    if (idTagInfo.expiryDate) {
        auto expiryDate_str = idTagInfo.expiryDate.value().to_rfc3339();
        sqlite3_bind_text(insert_statement, 3, expiryDate_str.c_str(), -1, SQLITE_TRANSIENT);
    }
    if (idTagInfo.parentIdTag) {
        auto parentIdTag_str = idTagInfo.parentIdTag.value().get();
        sqlite3_bind_text(insert_statement, 4, parentIdTag_str.c_str(), -1, SQLITE_TRANSIENT);
    }

    int res = sqlite3_step(insert_statement);
    if (res != SQLITE_DONE) {
        EVLOG(error) << "Could not insert into table: " << res << sqlite3_errmsg(db);
        throw std::runtime_error("db access error");
    }

    if (sqlite3_finalize(insert_statement) != SQLITE_OK) {
        EVLOG(error) << "Error inserting into table";
        throw std::runtime_error("db access error");
    }

    return true;
}

bool ChargePointConfiguration::clearAuthorizationCache() {
    if (!this->getAuthorizationCacheEnabled()) {
        return false;
    }

    std::string clear_sql_str = "DELETE FROM AUTH_CACHE;";
    sqlite3_stmt* clear_statement;
    sqlite3_prepare_v2(db, clear_sql_str.c_str(), clear_sql_str.size(), &clear_statement, NULL);
    int res = sqlite3_step(clear_statement);
    if (res != SQLITE_DONE) {
        EVLOG(error) << "Could not clear AUTH_CACHE table: " << res << sqlite3_errmsg(db);
        return false;
    }

    if (sqlite3_finalize(clear_statement) != SQLITE_OK) {
        EVLOG(error) << "Error clearing table";
        throw std::runtime_error("db access error");
    }

    return true;
}

boost::optional<IdTagInfo> ChargePointConfiguration::getAuthorizationCacheEntry(CiString20Type idTag) {
    if (!this->getAuthorizationCacheEnabled()) {
        return boost::none;
    }
    std::ostringstream select_sql;
    select_sql << "SELECT ID_TAG, AUTH_STATUS, EXPIRY_DATE, PARENT_ID_TAG FROM AUTH_CACHE WHERE ID_TAG = @id_tag";
    std::string select_sql_str = select_sql.str();
    sqlite3_stmt* select_statement;
    sqlite3_prepare_v2(db, select_sql_str.c_str(), select_sql_str.size(), &select_statement, NULL);

    auto idTag_str = idTag.get();
    sqlite3_bind_text(select_statement, 1, idTag_str.c_str(), -1, NULL);

    int res = sqlite3_step(select_statement);
    if (res != SQLITE_ROW) {
        // no idTag with that name exists in the cache
        return boost::none;
    }

    IdTagInfo idTagInfo;
    std::string auth_status_str = std::string(reinterpret_cast<const char*>(sqlite3_column_text(select_statement, 1)));
    EVLOG(error) << "auth_status_str: " << auth_status_str;
    idTagInfo.status = conversions::string_to_authorization_status(auth_status_str);
    auto expiry_date_ptr = sqlite3_column_text(select_statement, 2);
    if (expiry_date_ptr != nullptr) {
        std::string expiry_date_str = std::string(reinterpret_cast<const char*>(expiry_date_ptr));
        EVLOG(debug) << "expiry_date_str available: " << expiry_date_str;
        auto expiry_date = DateTime(expiry_date_str);
        idTagInfo.expiryDate.emplace(expiry_date);
    } else {
        EVLOG(debug) << "expiry_date_str not available";
    }

    auto parent_id_tag_ptr = sqlite3_column_text(select_statement, 3);
    if (parent_id_tag_ptr != nullptr) {
        std::string parent_id_tag_str = std::string(reinterpret_cast<const char*>(parent_id_tag_ptr));
        EVLOG(debug) << "parent_id_tag_str available: " << parent_id_tag_str;
        idTagInfo.parentIdTag.emplace(parent_id_tag_str);
    } else {
        EVLOG(debug) << "parent_id_tag_str not available";
    }

    if (sqlite3_finalize(select_statement) != SQLITE_OK) {
        EVLOG(error) << "Error selecting from table";
        throw std::runtime_error("db access error");
    }

    // check if expiry date is set and the entr should be set to Expired
    if (idTagInfo.status != AuthorizationStatus::Expired) {
        if (idTagInfo.expiryDate) {
            auto now = DateTime();
            if (idTagInfo.expiryDate.get() <= now) {
                EVLOG(info) << "IdTag " << idTag
                            << " in auth cache has expiry date in the past, setting entry to expired.";
                idTagInfo.status = AuthorizationStatus::Expired;
                this->updateAuthorizationCacheEntry(idTag, idTagInfo);
            }
        }
    }

    return idTagInfo;
}

// Core Profile start

// Core Profile - optional
boost::optional<bool> ChargePointConfiguration::getAllowOfflineTxForUnknownId() {
    boost::optional<bool> unknown_offline_auth = boost::none;
    if (this->config["Core"].contains("AllowOfflineTxForUnknownId")) {
        unknown_offline_auth.emplace(this->config["Core"]["AllowOfflineTxForUnknownId"]);
    }
    return unknown_offline_auth;
}
void ChargePointConfiguration::setAllowOfflineTxForUnknownId(bool enabled) {
    if (this->getAllowOfflineTxForUnknownId() != boost::none) {
        this->config["Core"]["AllowOfflineTxForUnknownId"] = enabled;
    }
}
boost::optional<KeyValue> ChargePointConfiguration::getAllowOfflineTxForUnknownIdKeyValue() {
    boost::optional<KeyValue> unknown_offline_auth_kv = boost::none;
    auto unknown_offline_auth = this->getAllowOfflineTxForUnknownId();
    if (unknown_offline_auth != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "AllowOfflineTxForUnknownId";
        kv.readonly = false;
        kv.value.emplace(conversions::bool_to_string(unknown_offline_auth.value()));
        unknown_offline_auth_kv.emplace(kv);
    }
    return unknown_offline_auth_kv;
}

// Core Profile - optional
boost::optional<bool> ChargePointConfiguration::getAuthorizationCacheEnabled() {
    boost::optional<bool> enabled = boost::none;
    if (this->config["Core"].contains("AuthorizationCacheEnabled")) {
        enabled.emplace(this->config["Core"]["AuthorizationCacheEnabled"]);
    }
    return enabled;
}
void ChargePointConfiguration::setAuthorizationCacheEnabled(bool enabled) {
    if (this->getAuthorizationCacheEnabled() != boost::none) {
        this->config["Core"]["AuthorizationCacheEnabled"] = enabled;
    }
}
boost::optional<KeyValue> ChargePointConfiguration::getAuthorizationCacheEnabledKeyValue() {
    boost::optional<KeyValue> enabled_kv = boost::none;
    auto enabled = this->getAuthorizationCacheEnabled();
    if (enabled != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "AuthorizationCacheEnabled";
        kv.readonly = false;
        kv.value.emplace(conversions::bool_to_string(enabled.value()));
        enabled_kv.emplace(kv);
    }
    return enabled_kv;
}

// Core Profile
bool ChargePointConfiguration::getAuthorizeRemoteTxRequests() {
    return this->config["Core"]["AuthorizeRemoteTxRequests"];
}
KeyValue ChargePointConfiguration::getAuthorizeRemoteTxRequestsKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "AuthorizeRemoteTxRequests";
    kv.readonly = true; // Could also be RW if we choose so
    kv.value.emplace(conversions::bool_to_string(this->getAuthorizeRemoteTxRequests()));
    return kv;
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getBlinkRepeat() {
    boost::optional<int32_t> blink_repeat = boost::none;
    if (this->config["Core"].contains("BlinkRepeat")) {
        blink_repeat.emplace(this->config["Core"]["BlinkRepeat"]);
    }
    return blink_repeat;
}
void ChargePointConfiguration::setBlinkRepeat(int32_t blink_repeat) {
    if (this->getBlinkRepeat() != boost::none) {
        this->config["Core"]["BlinkRepeat"] = blink_repeat;
    }
}
boost::optional<KeyValue> ChargePointConfiguration::getBlinkRepeatKeyValue() {
    boost::optional<KeyValue> blink_repeat_kv = boost::none;
    auto blink_repeat = this->getBlinkRepeat();
    if (blink_repeat != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "BlinkRepeat";
        kv.readonly = false;
        kv.value.emplace(std::to_string(blink_repeat.value()));
        blink_repeat_kv.emplace(kv);
    }
    return blink_repeat_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getClockAlignedDataInterval() {
    return this->config["Core"]["ClockAlignedDataInterval"];
}
void ChargePointConfiguration::setClockAlignedDataInterval(int32_t interval) {
    this->config["Core"]["ClockAlignedDataInterval"] = interval;
}
KeyValue ChargePointConfiguration::getClockAlignedDataIntervalKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "ClockAlignedDataInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getClockAlignedDataInterval()));
    return kv;
}

// Core Profile
int32_t ChargePointConfiguration::getConnectionTimeOut() {
    return this->config["Core"]["ConnectionTimeOut"];
}
void ChargePointConfiguration::setConnectionTimeOut(int32_t timeout) {
    this->config["Core"]["ConnectionTimeOut"] = timeout;
}
KeyValue ChargePointConfiguration::getConnectionTimeOutKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "ConnectionTimeOut";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getConnectionTimeOut()));
    return kv;
}

// Core Profile
std::string ChargePointConfiguration::getConnectorPhaseRotation() {
    return this->config["Core"]["ConnectorPhaseRotation"];
}
void ChargePointConfiguration::setConnectorPhaseRotation(std::string connector_phase_rotation) {
    this->config["Core"]["ConnectorPhaseRotation"] = connector_phase_rotation;
}
KeyValue ChargePointConfiguration::getConnectorPhaseRotationKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "ConnectorPhaseRotation";
    kv.readonly = false;
    kv.value.emplace(this->getConnectorPhaseRotation());
    return kv;
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getConnectorPhaseRotationMaxLength() {
    boost::optional<int32_t> max_length = boost::none;
    if (this->config["Core"].contains("ConnectorPhaseRotationMaxLength")) {
        max_length.emplace(this->config["Core"]["ConnectorPhaseRotationMaxLength"]);
    }
    return max_length;
}
boost::optional<KeyValue> ChargePointConfiguration::getConnectorPhaseRotationMaxLengthKeyValue() {
    boost::optional<KeyValue> max_length_kv = boost::none;
    auto max_length = this->getConnectorPhaseRotationMaxLength();
    if (max_length != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "ConnectorPhaseRotationMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getGetConfigurationMaxKeys() {
    return this->config["Core"]["GetConfigurationMaxKeys"];
}
KeyValue ChargePointConfiguration::getGetConfigurationMaxKeysKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "GetConfigurationMaxKeys";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getGetConfigurationMaxKeys()));
    return kv;
}

// Core Profile
int32_t ChargePointConfiguration::getHeartbeatInterval() {
    return this->config["Core"]["HeartbeatInterval"];
}
void ChargePointConfiguration::setHeartbeatInterval(int32_t interval) {
    this->config["Core"]["HeartbeatInterval"] = interval;
}
KeyValue ChargePointConfiguration::getHeartbeatIntervalKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "HeartbeatInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getHeartbeatInterval()));
    return kv;
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getLightIntensity() {
    boost::optional<int32_t> light_intensity = boost::none;
    if (this->config["Core"].contains("LightIntensity")) {
        light_intensity.emplace(this->config["Core"]["LightIntensity"]);
    }
    return light_intensity;
}
void ChargePointConfiguration::setLightIntensity(int32_t light_intensity) {
    if (this->getLightIntensity() != boost::none) {
        this->config["Core"]["LightIntensity"] = light_intensity;
    }
}
boost::optional<KeyValue> ChargePointConfiguration::getLightIntensityKeyValue() {
    boost::optional<KeyValue> light_intensity_kv = boost::none;
    auto light_intensity = this->getLightIntensity();
    if (light_intensity != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "LightIntensity";
        kv.readonly = false;
        kv.value.emplace(std::to_string(light_intensity.value()));
        light_intensity_kv.emplace(kv);
    }
    return light_intensity_kv;
}

// Core Profile
bool ChargePointConfiguration::getLocalAuthorizeOffline() {
    return this->config["Core"]["LocalAuthorizeOffline"];
}
void ChargePointConfiguration::setLocalAuthorizeOffline(bool local_authorize_offline) {
    this->config["Core"]["LocalAuthorizeOffline"] = local_authorize_offline;
}
KeyValue ChargePointConfiguration::getLocalAuthorizeOfflineKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "LocalAuthorizeOffline";
    kv.readonly = false;
    kv.value.emplace(conversions::bool_to_string(this->getLocalAuthorizeOffline()));
    return kv;
}

// Core Profile
bool ChargePointConfiguration::getLocalPreAuthorize() {
    return this->config["Core"]["LocalPreAuthorize"];
}
void ChargePointConfiguration::setLocalPreAuthorize(bool local_pre_authorize) {
    this->config["Core"]["LocalPreAuthorize"] = local_pre_authorize;
}
KeyValue ChargePointConfiguration::getLocalPreAuthorizeKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "LocalPreAuthorize";
    kv.readonly = false;
    kv.value.emplace(conversions::bool_to_string(this->getLocalPreAuthorize()));
    return kv;
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getMaxEnergyOnInvalidId() {
    boost::optional<int32_t> max_energy = boost::none;
    if (this->config["Core"].contains("MaxEnergyOnInvalidId")) {
        max_energy.emplace(this->config["Core"]["MaxEnergyOnInvalidId"]);
    }
    return max_energy;
}
void ChargePointConfiguration::setMaxEnergyOnInvalidId(int32_t max_energy) {
    if (this->getMaxEnergyOnInvalidId() != boost::none) {
        this->config["Core"]["MaxEnergyOnInvalidId"] = max_energy;
    }
}
boost::optional<KeyValue> ChargePointConfiguration::getMaxEnergyOnInvalidIdKeyValue() {
    boost::optional<KeyValue> max_energy_kv = boost::none;
    auto max_energy = this->getMaxEnergyOnInvalidId();
    if (max_energy != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "MaxEnergyOnInvalidId";
        kv.readonly = false;
        kv.value.emplace(std::to_string(max_energy.value()));
        max_energy_kv.emplace(kv);
    }
    return max_energy_kv;
}

// Core Profile
std::string ChargePointConfiguration::getMeterValuesAlignedData() {
    return this->config["Core"]["MeterValuesAlignedData"];
}
bool ChargePointConfiguration::setMeterValuesAlignedData(std::string meter_values_aligned_data) {
    if (!this->measurands_supported(meter_values_aligned_data)) {
        return false;
    }
    this->config["Core"]["MeterValuesAlignedData"] = meter_values_aligned_data;
    return true;
}
KeyValue ChargePointConfiguration::getMeterValuesAlignedDataKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "MeterValuesAlignedData";
    kv.readonly = false;
    kv.value.emplace(this->getMeterValuesAlignedData());
    return kv;
}
std::vector<MeasurandWithPhase> ChargePointConfiguration::getMeterValuesAlignedDataVector() {
    return this->csv_to_measurand_with_phase_vector(this->getMeterValuesAlignedData());
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getMeterValuesAlignedDataMaxLength() {
    boost::optional<int32_t> max_length = boost::none;
    if (this->config["Core"].contains("MeterValuesAlignedDataMaxLength")) {
        max_length.emplace(this->config["Core"]["MeterValuesAlignedDataMaxLength"]);
    }
    return max_length;
}
boost::optional<KeyValue> ChargePointConfiguration::getMeterValuesAlignedDataMaxLengthKeyValue() {
    boost::optional<KeyValue> max_length_kv = boost::none;
    auto max_length = this->getMeterValuesAlignedDataMaxLength();
    if (max_length != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "MeterValuesAlignedDataMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
std::string ChargePointConfiguration::getMeterValuesSampledData() {
    return this->config["Core"]["MeterValuesSampledData"];
}
bool ChargePointConfiguration::setMeterValuesSampledData(std::string meter_values_sampled_data) {
    if (!this->measurands_supported(meter_values_sampled_data)) {
        return false;
    }
    this->config["Core"]["MeterValuesSampledData"] = meter_values_sampled_data;
    return true;
}
KeyValue ChargePointConfiguration::getMeterValuesSampledDataKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "MeterValuesSampledData";
    kv.readonly = false;
    kv.value.emplace(this->getMeterValuesSampledData());
    return kv;
}
std::vector<MeasurandWithPhase> ChargePointConfiguration::getMeterValuesSampledDataVector() {
    return this->csv_to_measurand_with_phase_vector(this->getMeterValuesSampledData());
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getMeterValuesSampledDataMaxLength() {
    boost::optional<int32_t> max_length = boost::none;
    if (this->config["Core"].contains("MeterValuesSampledDataMaxLength")) {
        max_length.emplace(this->config["Core"]["MeterValuesSampledDataMaxLength"]);
    }
    return max_length;
}
boost::optional<KeyValue> ChargePointConfiguration::getMeterValuesSampledDataMaxLengthKeyValue() {
    boost::optional<KeyValue> max_length_kv = boost::none;
    auto max_length = this->getMeterValuesSampledDataMaxLength();
    if (max_length != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "MeterValuesSampledDataMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getMeterValueSampleInterval() {
    return this->config["Core"]["MeterValueSampleInterval"];
}
void ChargePointConfiguration::setMeterValueSampleInterval(int32_t interval) {
    this->config["Core"]["MeterValueSampleInterval"] = interval;
}
KeyValue ChargePointConfiguration::getMeterValueSampleIntervalKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "MeterValueSampleInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getMeterValueSampleInterval()));
    return kv;
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getMinimumStatusDuration() {
    boost::optional<int32_t> minimum_status_duration = boost::none;
    if (this->config["Core"].contains("MinimumStatusDuration")) {
        minimum_status_duration.emplace(this->config["Core"]["MinimumStatusDuration"]);
    }
    return minimum_status_duration;
}
void ChargePointConfiguration::setMinimumStatusDuration(int32_t minimum_status_duration) {
    if (this->getMinimumStatusDuration() != boost::none) {
        this->config["Core"]["MinimumStatusDuration"] = minimum_status_duration;
    }
}
boost::optional<KeyValue> ChargePointConfiguration::getMinimumStatusDurationKeyValue() {
    boost::optional<KeyValue> minimum_status_duration_kv = boost::none;
    auto minimum_status_duration = this->getMinimumStatusDuration();
    if (minimum_status_duration != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "MinimumStatusDuration";
        kv.readonly = false;
        kv.value.emplace(std::to_string(minimum_status_duration.value()));
        minimum_status_duration_kv.emplace(kv);
    }
    return minimum_status_duration_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getNumberOfConnectors() {
    return this->config["Core"]["NumberOfConnectors"];
}
KeyValue ChargePointConfiguration::getNumberOfConnectorsKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "NumberOfConnectors";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getNumberOfConnectors()));
    return kv;
}

// Core Profile
int32_t ChargePointConfiguration::getResetRetries() {
    return this->config["Core"]["ResetRetries"];
}
void ChargePointConfiguration::setResetRetries(int32_t retries) {
    this->config["Core"]["ResetRetries"] = retries;
}
KeyValue ChargePointConfiguration::getResetRetriesKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "ResetRetries";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getResetRetries()));
    return kv;
}

// Core Profile
bool ChargePointConfiguration::getStopTransactionOnEVSideDisconnect() {
    return this->config["Core"]["StopTransactionOnEVSideDisconnect"];
}
void ChargePointConfiguration::setStopTransactionOnEVSideDisconnect(bool stop_transaction_on_ev_side_disconnect) {
    this->config["Core"]["StopTransactionOnEVSideDisconnect"] = stop_transaction_on_ev_side_disconnect;
}
KeyValue ChargePointConfiguration::getStopTransactionOnEVSideDisconnectKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "StopTransactionOnEVSideDisconnect";
    kv.readonly = false;
    kv.value.emplace(conversions::bool_to_string(this->getStopTransactionOnEVSideDisconnect()));
    return kv;
}

// Core Profile
bool ChargePointConfiguration::getStopTransactionOnInvalidId() {
    return this->config["Core"]["StopTransactionOnInvalidId"];
}
void ChargePointConfiguration::setStopTransactionOnInvalidId(bool stop_transaction_on_invalid_id) {
    this->config["Core"]["StopTransactionOnInvalidId"] = stop_transaction_on_invalid_id;
}
KeyValue ChargePointConfiguration::getStopTransactionOnInvalidIdKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "StopTransactionOnInvalidId";
    kv.readonly = false;
    kv.value.emplace(conversions::bool_to_string(this->getStopTransactionOnInvalidId()));
    return kv;
}

// Core Profile
std::string ChargePointConfiguration::getStopTxnAlignedData() {
    return this->config["Core"]["StopTxnAlignedData"];
}
bool ChargePointConfiguration::setStopTxnAlignedData(std::string stop_txn_aligned_data) {
    if (!this->measurands_supported(stop_txn_aligned_data)) {
        return false;
    }
    this->config["Core"]["StopTxnAlignedData"] = stop_txn_aligned_data;
    return true;
}
KeyValue ChargePointConfiguration::getStopTxnAlignedDataKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "StopTxnAlignedData";
    kv.readonly = false;
    kv.value.emplace(this->getStopTxnAlignedData());
    return kv;
}
std::vector<MeasurandWithPhase> ChargePointConfiguration::getStopTxnAlignedDataVector() {
    return this->csv_to_measurand_with_phase_vector(this->getStopTxnAlignedData());
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getStopTxnAlignedDataMaxLength() {
    boost::optional<int32_t> max_length = boost::none;
    if (this->config["Core"].contains("StopTxnAlignedDataMaxLength")) {
        max_length.emplace(this->config["Core"]["StopTxnAlignedDataMaxLength"]);
    }
    return max_length;
}
boost::optional<KeyValue> ChargePointConfiguration::getStopTxnAlignedDataMaxLengthKeyValue() {
    boost::optional<KeyValue> max_length_kv = boost::none;
    auto max_length = this->getStopTxnAlignedDataMaxLength();
    if (max_length != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "StopTxnAlignedDataMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
std::string ChargePointConfiguration::getStopTxnSampledData() {
    return this->config["Core"]["StopTxnSampledData"];
}
bool ChargePointConfiguration::setStopTxnSampledData(std::string stop_txn_sampled_data) {
    if (!this->measurands_supported(stop_txn_sampled_data)) {
        return false;
    }
    this->config["Core"]["StopTxnSampledData"] = stop_txn_sampled_data;
    return true;
}
KeyValue ChargePointConfiguration::getStopTxnSampledDataKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "StopTxnSampledData";
    kv.readonly = false;
    kv.value.emplace(this->getStopTxnSampledData());
    return kv;
}
std::vector<MeasurandWithPhase> ChargePointConfiguration::getStopTxnSampledDataVector() {
    return this->csv_to_measurand_with_phase_vector(this->getStopTxnSampledData());
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getStopTxnSampledDataMaxLength() {
    boost::optional<int32_t> max_length = boost::none;
    if (this->config["Core"].contains("StopTxnSampledDataMaxLength")) {
        max_length.emplace(this->config["Core"]["StopTxnSampledDataMaxLength"]);
    }
    return max_length;
}
boost::optional<KeyValue> ChargePointConfiguration::getStopTxnSampledDataMaxLengthKeyValue() {
    boost::optional<KeyValue> max_length_kv = boost::none;
    auto max_length = this->getStopTxnSampledDataMaxLength();
    if (max_length != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "StopTxnSampledDataMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
std::string ChargePointConfiguration::getSupportedFeatureProfiles() {
    return this->config["Core"]["SupportedFeatureProfiles"];
}
KeyValue ChargePointConfiguration::getSupportedFeatureProfilesKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "SupportedFeatureProfiles";
    kv.readonly = true;
    kv.value.emplace(this->getSupportedFeatureProfiles());
    return kv;
}
std::set<SupportedFeatureProfiles> ChargePointConfiguration::getSupportedFeatureProfilesSet() {
    return this->supported_feature_profiles;
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getSupportedFeatureProfilesMaxLength() {
    boost::optional<int32_t> max_length = boost::none;
    if (this->config["Core"].contains("SupportedFeatureProfilesMaxLength")) {
        max_length.emplace(this->config["Core"]["SupportedFeatureProfilesMaxLength"]);
    }
    return max_length;
}
boost::optional<KeyValue> ChargePointConfiguration::getSupportedFeatureProfilesMaxLengthKeyValue() {
    boost::optional<KeyValue> max_length_kv = boost::none;
    auto max_length = this->getSupportedFeatureProfilesMaxLength();
    if (max_length != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "SupportedFeatureProfilesMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getTransactionMessageAttempts() {
    return this->config["Core"]["TransactionMessageAttempts"];
}
void ChargePointConfiguration::setTransactionMessageAttempts(int32_t attempts) {
    this->config["Core"]["TransactionMessageAttempts"] = attempts;
}
KeyValue ChargePointConfiguration::getTransactionMessageAttemptsKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "TransactionMessageAttempts";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getTransactionMessageAttempts()));
    return kv;
}

// Core Profile
int32_t ChargePointConfiguration::getTransactionMessageRetryInterval() {
    return this->config["Core"]["TransactionMessageRetryInterval"];
}
void ChargePointConfiguration::setTransactionMessageRetryInterval(int32_t retry_interval) {
    this->config["Core"]["TransactionMessageRetryInterval"] = retry_interval;
}
KeyValue ChargePointConfiguration::getTransactionMessageRetryIntervalKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "TransactionMessageRetryInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getTransactionMessageRetryInterval()));
    return kv;
}

// Core Profile
bool ChargePointConfiguration::getUnlockConnectorOnEVSideDisconnect() {
    return this->config["Core"]["UnlockConnectorOnEVSideDisconnect"];
}
void ChargePointConfiguration::setUnlockConnectorOnEVSideDisconnect(bool unlock_connector_on_ev_side_disconnect) {
    this->config["Core"]["UnlockConnectorOnEVSideDisconnect"] = unlock_connector_on_ev_side_disconnect;
}
KeyValue ChargePointConfiguration::getUnlockConnectorOnEVSideDisconnectKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "UnlockConnectorOnEVSideDisconnect";
    kv.readonly = false;
    kv.value.emplace(conversions::bool_to_string(this->getUnlockConnectorOnEVSideDisconnect()));
    return kv;
}

// Core Profile - optional
boost::optional<int32_t> ChargePointConfiguration::getWebsocketPingInterval() {
    boost::optional<int32_t> websocket_ping_interval = boost::none;
    if (this->config["Core"].contains("WebsocketPingInterval")) {
        websocket_ping_interval.emplace(this->config["Core"]["WebsocketPingInterval"]);
    }
    return websocket_ping_interval;
}
void ChargePointConfiguration::setWebsocketPingInterval(int32_t websocket_ping_interval) {
    if (this->getWebsocketPingInterval() != boost::none) {
        this->config["Core"]["WebsocketPingInterval"] = websocket_ping_interval;
    }
}
boost::optional<KeyValue> ChargePointConfiguration::getWebsocketPingIntervalKeyValue() {
    boost::optional<KeyValue> websocket_ping_interval_kv = boost::none;
    auto websocket_ping_interval = this->getWebsocketPingInterval();
    if (websocket_ping_interval != boost::none) {
        ocpp1_6::KeyValue kv;
        kv.key = "WebsocketPingInterval";
        kv.readonly = false;
        kv.value.emplace(std::to_string(websocket_ping_interval.value()));
        websocket_ping_interval_kv.emplace(kv);
    }
    return websocket_ping_interval_kv;
}

// Core Profile end

int32_t ChargePointConfiguration::getChargeProfileMaxStackLevel() {
    return this->config["SmartCharging"]["ChargeProfileMaxStackLevel"];
}
KeyValue ChargePointConfiguration::getChargeProfileMaxStackLevelKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "ChargeProfileMaxStackLevel";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getChargeProfileMaxStackLevel()));
    return kv;
}

std::string ChargePointConfiguration::getChargingScheduleAllowedChargingRateUnit() {
    return this->config["SmartCharging"]["ChargingScheduleAllowedChargingRateUnit"];
}
KeyValue ChargePointConfiguration::getChargingScheduleAllowedChargingRateUnitKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "ChargingScheduleAllowedChargingRateUnit";
    kv.readonly = true;
    kv.value.emplace(this->getChargingScheduleAllowedChargingRateUnit());
    return kv;
}

int32_t ChargePointConfiguration::getChargingScheduleMaxPeriods() {
    return this->config["SmartCharging"]["ChargingScheduleMaxPeriods"];
}
KeyValue ChargePointConfiguration::getChargingScheduleMaxPeriodsKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "ChargingScheduleMaxPeriods";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getChargingScheduleMaxPeriods()));
    return kv;
}

bool ChargePointConfiguration::getConnectorSwitch3to1PhaseSupported() {
    return this->config["SmartCharging"]["ConnectorSwitch3to1PhaseSupported"];
}
KeyValue ChargePointConfiguration::getConnectorSwitch3to1PhaseSupportedKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "ConnectorSwitch3to1PhaseSupported";
    kv.readonly = true;
    kv.value.emplace(conversions::bool_to_string(this->getConnectorSwitch3to1PhaseSupported()));
    return kv;
}

int32_t ChargePointConfiguration::getMaxChargingProfilesInstalled() {
    return this->config["SmartCharging"]["MaxChargingProfilesInstalled"];
}
KeyValue ChargePointConfiguration::getMaxChargingProfilesInstalledKeyValue() {
    ocpp1_6::KeyValue kv;
    kv.key = "MaxChargingProfilesInstalled";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getMaxChargingProfilesInstalled()));
    return kv;
}

boost::optional<KeyValue> ChargePointConfiguration::get(CiString50Type key) {
    if (key == "AllowOfflineTxForUnknownId") {
        return this->getAllowOfflineTxForUnknownIdKeyValue();
    }
    if (key == "AuthorizationCacheEnabled") {
        return this->getAuthorizationCacheEnabledKeyValue();
    }
    if (key == "AuthorizeRemoteTxRequests") {
        return this->getAuthorizeRemoteTxRequestsKeyValue();
    }
    if (key == "BlinkRepeat") {
        return this->getBlinkRepeatKeyValue();
    }
    if (key == "ClockAlignedDataInterval") {
        return this->getClockAlignedDataIntervalKeyValue();
    }
    if (key == "ConnectionTimeOut") {
        return this->getConnectionTimeOutKeyValue();
    }
    if (key == "ConnectorPhaseRotation") {
        return this->getConnectorPhaseRotationKeyValue();
    }
    if (key == "ConnectorPhaseRotationMaxLength") {
        return this->getConnectorPhaseRotationMaxLengthKeyValue();
    }
    if (key == "GetConfigurationMaxKeys") {
        return this->getGetConfigurationMaxKeysKeyValue();
    }
    if (key == "HeartbeatInterval") {
        return this->getHeartbeatIntervalKeyValue();
    }
    if (key == "LightIntensity") {
        return this->getLightIntensityKeyValue();
    }
    if (key == "LocalAuthorizeOffline") {
        return this->getLocalAuthorizeOfflineKeyValue();
    }
    if (key == "LocalPreAuthorize") {
        return this->getLocalPreAuthorizeKeyValue();
    }
    if (key == "MaxEnergyOnInvalidId") {
        return this->getMaxEnergyOnInvalidIdKeyValue();
    }
    if (key == "MeterValuesAlignedData") {
        return this->getMeterValuesAlignedDataKeyValue();
    }
    if (key == "MeterValuesAlignedDataMaxLength") {
        return this->getMeterValuesAlignedDataMaxLengthKeyValue();
    }
    if (key == "MeterValuesSampledData") {
        return this->getMeterValuesSampledDataKeyValue();
    }
    if (key == "MeterValuesSampledDataMaxLength") {
        return this->getMeterValuesSampledDataMaxLengthKeyValue();
    }
    if (key == "MeterValueSampleInterval") {
        return this->getMeterValueSampleIntervalKeyValue();
    }
    if (key == "MinimumStatusDuration") {
        return this->getMinimumStatusDurationKeyValue();
    }
    if (key == "NumberOfConnectors") {
        return this->getNumberOfConnectorsKeyValue();
    }
    if (key == "ResetRetries") {
        return this->getResetRetriesKeyValue();
    }
    if (key == "StopTransactionOnEVSideDisconnect") {
        return this->getStopTransactionOnEVSideDisconnectKeyValue();
    }
    if (key == "StopTransactionOnInvalidId") {
        return this->getStopTransactionOnInvalidIdKeyValue();
    }
    if (key == "StopTxnAlignedData") {
        return this->getStopTxnAlignedDataKeyValue();
    }
    if (key == "StopTxnAlignedDataMaxLength") {
        return this->getStopTxnAlignedDataMaxLengthKeyValue();
    }
    if (key == "StopTxnSampledData") {
        return this->getStopTxnSampledDataKeyValue();
    }
    if (key == "StopTxnSampledDataMaxLength") {
        return this->getStopTxnSampledDataMaxLengthKeyValue();
    }
    if (key == "SupportedFeatureProfiles") {
        return this->getSupportedFeatureProfilesKeyValue();
    }
    if (key == "SupportedFeatureProfilesMaxLength") {
        return this->getSupportedFeatureProfilesMaxLengthKeyValue();
    }
    if (key == "TransactionMessageAttempts") {
        return this->getTransactionMessageAttemptsKeyValue();
    }
    if (key == "TransactionMessageRetryInterval") {
        return this->getTransactionMessageRetryIntervalKeyValue();
    }
    if (key == "UnlockConnectorOnEVSideDisconnect") {
        return this->getUnlockConnectorOnEVSideDisconnectKeyValue();
    }
    if (key == "WebsocketPingInterval") {
        return this->getWebsocketPingIntervalKeyValue();
    }
    // if (key == "ChargeProfileMaxStackLevel") {
    //     return this->getChargeProfileMaxStackLevelKeyValue();
    // }
    // if (key == "ChargingScheduleAllowedChargingRateUnit") {
    //     return this->getChargingScheduleAllowedChargingRateUnitKeyValue();
    // }
    // if (key == "ChargingScheduleMaxPeriods") {
    //     return this->getChargingScheduleMaxPeriodsKeyValue();
    // }
    // if (key == "ConnectorSwitch3to1PhaseSupported") {
    //     return this->getConnectorSwitch3to1PhaseSupportedKeyValue();
    // }
    // if (key == "MaxChargingProfilesInstalled") {
    //     return this->getMaxChargingProfilesInstalledKeyValue();
    // }

    return boost::none;
}

std::vector<KeyValue> ChargePointConfiguration::get_all_key_value() {
    std::vector<KeyValue> all;
    for (auto feature_profile : this->getSupportedFeatureProfilesSet()) {
        auto feature_profile_string = conversions::supported_feature_profiles_to_string(feature_profile);
        if (this->config.contains(feature_profile_string)) {
            auto& feature_config = this->config[feature_profile_string];
            for (auto& feature_config_entry : feature_config.items()) {
                auto config_key = CiString50Type(feature_config_entry.key());
                auto config_value = this->get(config_key);
                if (config_value != boost::none) {
                    all.push_back(config_value.value());
                }
            }
        }
    }
    return all;
}

ConfigurationStatus ChargePointConfiguration::set(CiString50Type key, CiString500Type value) {
    if (key == "AllowOfflineTxForUnknownId") {
        if (this->getAllowOfflineTxForUnknownId() == boost::none) {
            return ConfigurationStatus::NotSupported;
        }
        this->setAllowOfflineTxForUnknownId(conversions::string_to_bool(value.get()));
    }
    if (key == "AuthorizationCacheEnabled") {
        if (this->getAuthorizationCacheEnabled() == boost::none) {
            return ConfigurationStatus::NotSupported;
        }
        this->setAuthorizationCacheEnabled(conversions::string_to_bool(value.get()));
    }
    // TODO(kai): Implementations can choose if the is R or RW, at the moment readonly
    // if (key == "AuthorizeRemoteTxRequests") {
    //     this->setAuthorizeRemoteTxRequests(conversions::string_to_bool(value.get()));
    // }
    if (key == "BlinkRepeat") {
        if (this->getBlinkRepeat() == boost::none) {
            return ConfigurationStatus::NotSupported;
        }
        auto blink_repeat = std::stoi(value.get());
        if (blink_repeat < 0) {
            return ConfigurationStatus::Rejected;
        }
        this->setBlinkRepeat(blink_repeat);
    }
    if (key == "ClockAlignedDataInterval") {
        auto interval = std::stoi(value.get());
        if (interval < 0) {
            return ConfigurationStatus::Rejected;
        }
        this->setClockAlignedDataInterval(interval);
    }
    if (key == "ConnectionTimeOut") {
        auto interval = std::stoi(value.get());
        if (interval < 0) {
            return ConfigurationStatus::Rejected;
        }
        this->setConnectionTimeOut(interval);
    }
    if (key == "ConnectorPhaseRotation") {
        this->setConnectorPhaseRotation(value.get());
    }
    if (key == "HeartbeatInterval") {
        auto interval = std::stoi(value.get());
        if (interval < 0) {
            return ConfigurationStatus::Rejected;
        }
        this->setHeartbeatInterval(interval);
    }
    if (key == "LightIntensity") {
        if (this->getLightIntensity() == boost::none) {
            return ConfigurationStatus::NotSupported;
        }
        auto light_intensity = std::stoi(value.get());
        if (light_intensity < 0 || light_intensity > 100) {
            return ConfigurationStatus::Rejected;
        }
        this->setLightIntensity(light_intensity);
    }
    if (key == "LocalAuthorizeOffline") {
        this->setLocalAuthorizeOffline(conversions::string_to_bool(value.get()));
    }
    if (key == "LocalPreAuthorize") {
        this->setLocalPreAuthorize(conversions::string_to_bool(value.get()));
    }
    if (key == "MaxEnergyOnInvalidId") {
        if (this->getMaxEnergyOnInvalidId() == boost::none) {
            return ConfigurationStatus::NotSupported;
        }
        auto max_energy = std::stoi(value.get());
        if (max_energy < 0) {
            return ConfigurationStatus::Rejected;
        }
        this->setMaxEnergyOnInvalidId(max_energy);
    }
    if (key == "MeterValuesAlignedData") {
        if (!this->setMeterValuesAlignedData(value.get())) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "MeterValuesSampledData") {
        if (!this->setMeterValuesSampledData(value.get())) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "MeterValueSampleInterval") {
        this->setMeterValueSampleInterval(std::stoi(value.get()));
    }
    if (key == "MinimumStatusDuration") {
        if (this->getMinimumStatusDuration() == boost::none) {
            return ConfigurationStatus::NotSupported;
        }
        auto duration = std::stoi(value.get());
        if (duration < 0) {
            return ConfigurationStatus::Rejected;
        }
        this->setMinimumStatusDuration(duration);
    }
    if (key == "ResetRetries") {
        this->setResetRetries(std::stoi(value.get()));
    }
    if (key == "StopTransactionOnEVSideDisconnect") {
        this->setStopTransactionOnEVSideDisconnect(conversions::string_to_bool(value.get()));
    }
    if (key == "StopTransactionOnInvalidId") {
        this->setStopTransactionOnInvalidId(conversions::string_to_bool(value.get()));
    }
    if (key == "StopTxnAlignedData") {
        if (!this->setStopTxnAlignedData(value.get())) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "StopTxnSampledData") {
        if (!this->setStopTxnSampledData(value.get())) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "TransactionMessageAttempts") {
        this->setTransactionMessageAttempts(std::stoi(value.get()));
    }
    if (key == "TransactionMessageRetryInterval") {
        this->setTransactionMessageRetryInterval(std::stoi(value.get()));
    }
    if (key == "UnlockConnectorOnEVSideDisconnect") {
        this->setUnlockConnectorOnEVSideDisconnect(conversions::string_to_bool(value.get()));
    }
    if (key == "WebsocketPingInterval") {
        if (this->getWebsocketPingInterval() == boost::none) {
            return ConfigurationStatus::NotSupported;
        }
        auto interval = std::stoi(value.get());
        if (interval < 0) {
            return ConfigurationStatus::Rejected;
        }
        this->setWebsocketPingInterval(interval);
    }

    return ConfigurationStatus::Accepted;
}
} // namespace ocpp1_6
