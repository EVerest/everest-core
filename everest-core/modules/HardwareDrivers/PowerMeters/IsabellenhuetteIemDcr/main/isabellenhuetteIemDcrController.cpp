// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "isabellenhuetteIemDcrController.hpp"
#include <stdexcept>
namespace module::main {

IsaIemDcrController::IsaIemDcrController(std::unique_ptr<HttpClientInterface> http_client,
                                         const SnapshotConfig& snap_config) :
    http_client(std::move(http_client)), snapshot_config(snap_config) {
    // Member Initializer List is used

    // Further initialization
    zone_time_offset = helper_convert_timezone(snapshot_config.timezone);
    last_datetime_sync.store(std::chrono::steady_clock::now() - std::chrono::hours(48));
}

bool IsaIemDcrController::init() {
    try {
        EVLOG_info << "Isabellenhuette IEM-DCR: Connecting to module...";
        // Check connection with polling REST node gw
        this->get_gw();
        // Send gw information
        try {
            this->post_gw();
        } catch (IsaIemDcrController::UnexpectedIemDcrResponseCode& error) {
            EVLOG_warning << "Node /gw seems to be already set. If those values should be updated, "
                             "please restart IEM-DCR and then also this system.";
        }
        // Send initial tariff information
        try {
            if (snapshot_config.TT_initial.length() > 0) {
                this->post_tariff(snapshot_config.TT_initial);
            }
        } catch (IsaIemDcrController::UnexpectedIemDcrResponseCode& error) {
            EVLOG_warning << "Incorrect config: Value TT_initial could not be set. Please check its value.";
        }
        EVLOG_info << "Isabellenhuette IEM-DCR: Connected.";
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

json IsaIemDcrController::get_gw() {
    const std::string endpoint = "/counter/v1/ocmf/gw";
    auto response = this->http_client->get(endpoint);
    if (response.status_code == 200) {
        try {
            json data = json::parse(response.body);
            return data;
        } catch (json::exception& json_error) {
            throw UnexpectedIemDcrResponseBody(
                endpoint, fmt::format("Json error {} for body {}", json_error.what(), response.body));
        }
    } else {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

bool IsaIemDcrController::check_gw_is_empty() {
    json gw_result = this->get_gw();
    return gw_result.at("CT").empty();
}

void IsaIemDcrController::post_gw() {
    const std::string endpoint = "/counter/v1/ocmf/gw";
    const std::string payload = nlohmann::ordered_json{{"CT", snapshot_config.CT},
                                                       {"CI", snapshot_config.CI},
                                                       {"TM", helper_get_current_datetime()}}
                                    .dump();
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code == 200) {
        last_datetime_sync.store(std::chrono::steady_clock::now());
    } else {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

void IsaIemDcrController::post_tariff(std::string tariff_info) {
    const std::string endpoint = "/counter/v1/ocmf/tariff";
    const std::string payload = nlohmann::ordered_json{{"TT", tariff_info}}.dump();
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

std::tuple<types::powermeter::Powermeter, std::string, bool> IsaIemDcrController::get_metervalue() {
    const std::string endpoint = "/counter/v1/ocmf/metervalue";
    auto response = this->http_client->get(endpoint);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
    try {
        json data = json::parse(response.body);

        types::powermeter::Powermeter powermeter;
        bool tmp_transaction_active = data.at("XT");
        powermeter.timestamp = data.at("TM");
        // Remove format specifier at the end (if available)
        if (powermeter.timestamp.length() > 28) {
            powermeter.timestamp = powermeter.timestamp.substr(0, 28);
        }
        powermeter.meter_id = data.at("MS");
        auto current = types::units::Current{};
        current.DC = data.at("I");
        powermeter.current_A.emplace(current);
        auto voltageU2 = types::units::Voltage{};
        voltageU2.DC = data.at("U2");
        powermeter.voltage_V.emplace(voltageU2);
        powermeter.power_W.emplace(types::units::Power{data.at("P").get<float>()});
        // Remove quotes before casting to float
        auto energy_kWh_import = helper_remove_first_and_last_char(data.at("RD").at(2).at("WV"));
        powermeter.energy_Wh_import = {std::stof(energy_kWh_import) * 1000.0f};
        // Remove quotes before casting to float
        auto energy_kWh_export = helper_remove_first_and_last_char(data.at("RD").at(3).at("WV"));
        powermeter.energy_Wh_export = {std::stof(energy_kWh_export) * 1000.0f};
        // Get status
        std::string status = data.at("XC");

        return std::make_tuple(powermeter, status, tmp_transaction_active);
    } catch (json::exception& json_error) {
        throw UnexpectedIemDcrResponseBody(endpoint,
                                           fmt::format("Json error {} for body {}", json_error.what(), response.body));
    }
}

std::string IsaIemDcrController::get_publickey(bool allow_cached_value) {
    if (allow_cached_value && cached_public_key.length() > 0) {
        return cached_public_key;
    } else {
        const std::string endpoint = "/counter/v1/ocmf/publickey";
        auto response = this->http_client->get(endpoint);
        if (response.status_code != 200) {
            EVLOG_warning << "Response to retrieval of public key is not 200." << std::endl;
            return "";
        }
        try {
            json data = json::parse(response.body);
            cached_public_key = data.at("PK");
            return cached_public_key;
        } catch (json::exception& json_error) {
            EVLOG_warning << "JSON error during parsing of public key" << std::endl;
            return "";
        }
    }
}

std::string IsaIemDcrController::get_datetime() {
    const std::string endpoint = "/counter/v1/ocmf/datetime";
    auto response = this->http_client->get(endpoint);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
    try {
        json data = json::parse(response.body);
        return data.at("TM");
    } catch (json::exception& json_error) {
        throw UnexpectedIemDcrResponseBody(endpoint,
                                           fmt::format("Json error {} for body {}", json_error.what(), response.body));
    }
}

void IsaIemDcrController::post_datetime() {
    const std::string endpoint = "/counter/v1/ocmf/datetime";
    const std::string payload = nlohmann::ordered_json{{"TM", helper_get_current_datetime()}}.dump();
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code == 200) {
        last_datetime_sync.store(std::chrono::steady_clock::now());
    } else {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

void IsaIemDcrController::refresh_datetime_if_required() {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::hours>(now - last_datetime_sync.load());
    if (elapsed.count() >= snapshot_config.datetime_resync_interval) {
        try {
            this->post_datetime();
            EVLOG_info << "DateTime resynchronized.";
        } catch (...) {
            // On error: just retry on next call
        }
    }
}

void IsaIemDcrController::post_user(const types::powermeter::OCMFUserIdentificationStatus IS,
                                    const std::optional<types::powermeter::OCMFIdentificationLevel> IL,
                                    const std::vector<types::powermeter::OCMFIdentificationFlags>& IF,
                                    const types::powermeter::OCMFIdentificationType& IT,
                                    const std::optional<std::__cxx11::basic_string<char>>& ID,
                                    const std::optional<std::__cxx11::basic_string<char>>& TT) {

    const std::string endpoint = "/counter/v1/ocmf/user";
    bool boolIS = helper_get_bool_from_OCMFUserIdentificationStatus(IS);
    std::string strIL = helper_get_string_from_OCMFIdentificationLevel(IL);
    std::string strIT = helper_get_string_from_OCMFIdentificationType(IT);
    std::string strID = static_cast<std::string>(ID.value_or(""));
    std::string strTT = static_cast<std::string>(TT.value_or(""));
    std::string payload = "";
    std::vector<std::string> vectIF;

    // Fill vectIF
    for (const types::powermeter::OCMFIdentificationFlags& id_flag : IF) {
        vectIF.push_back(helper_get_string_from_OCMFIdentificationFlags(id_flag));
    }

    if (strTT.length() > 0) {
        payload = nlohmann::ordered_json{{"IS", boolIS}, {"IL", strIL}, {"IF", vectIF},
                                         {"IT", strIT},  {"ID", strID}, {"US", snapshot_config.US},
                                         {"TT", strTT}}
                      .dump();
    } else {
        payload = nlohmann::ordered_json{{"IS", boolIS}, {"IL", strIL}, {"IF", vectIF},
                                         {"IT", strIT},  {"ID", strID}, {"US", snapshot_config.US}}
                      .dump();
    }
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

types::units_signed::SignedMeterValue IsaIemDcrController::get_receipt() {
    const std::string endpoint = "/counter/v1/ocmf/receipt";
    return helper_get_signed_datatuple(endpoint);
}

types::units_signed::SignedMeterValue IsaIemDcrController::get_transaction() {
    try {
        const std::string endpoint = "/counter/v1/ocmf/transaction";
        return helper_get_signed_datatuple(endpoint);
    } catch (UnexpectedIemDcrResponseCode& resp_error) {
        // Retry with newer api endpoint
        const std::string endpoint_v2 = "/counter/v2/ocmf/transaction";
        return helper_get_signed_datatuple(endpoint_v2);
    }
}

void IsaIemDcrController::post_receipt(const std::string& TX) {
    const std::string endpoint = "/counter/v1/ocmf/receipt";
    const std::string payload = nlohmann::ordered_json{{"TX", TX}}.dump();
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

bool IsaIemDcrController::helper_get_bool_from_OCMFUserIdentificationStatus(
    types::powermeter::OCMFUserIdentificationStatus IS) {
    return (IS == types::powermeter::OCMFUserIdentificationStatus::ASSIGNED);
}

std::string IsaIemDcrController::helper_get_string_from_OCMFIdentificationLevel(
    std::optional<types::powermeter::OCMFIdentificationLevel> IL) {
    std::string result;
    types::powermeter::OCMFIdentificationLevel value_IL =
        IL.value_or(types::powermeter::OCMFIdentificationLevel::UNKNOWN);
    switch (value_IL) {
    case types::powermeter::OCMFIdentificationLevel::NONE:
        result = "NONE";
        break;
    case types::powermeter::OCMFIdentificationLevel::HEARSAY:
        result = "HEARSAY";
        break;
    case types::powermeter::OCMFIdentificationLevel::TRUSTED:
        result = "TRUSTED";
        break;
    case types::powermeter::OCMFIdentificationLevel::VERIFIED:
        result = "VERIFIED";
        break;
    case types::powermeter::OCMFIdentificationLevel::CERTIFIED:
        result = "CERTIFIED";
        break;
    case types::powermeter::OCMFIdentificationLevel::SECURE:
        result = "SECURE";
        break;
    case types::powermeter::OCMFIdentificationLevel::MISMATCH:
        result = "MISMATCH";
        break;
    case types::powermeter::OCMFIdentificationLevel::INVALID:
        result = "INVALID";
        break;
    case types::powermeter::OCMFIdentificationLevel::OUTDATED:
        result = "OUTDATED";
        break;
    default:
        result = "UNKNOWN";
        break;
    }
    return result;
}

std::string IsaIemDcrController::helper_get_string_from_OCMFIdentificationFlags(
    types::powermeter::OCMFIdentificationFlags id_flag) {
    std::string result;
    switch (id_flag) {
    case types::powermeter::OCMFIdentificationFlags::RFID_NONE:
        result = "RFID_NONE";
        break;
    case types::powermeter::OCMFIdentificationFlags::RFID_PLAIN:
        result = "RFID_PLAIN";
        break;
    case types::powermeter::OCMFIdentificationFlags::RFID_RELATED:
        result = "RFID_RELATED";
        break;
    case types::powermeter::OCMFIdentificationFlags::RFID_PSK:
        result = "RFID_PSK";
        break;
    case types::powermeter::OCMFIdentificationFlags::OCPP_NONE:
        result = "OCPP_NONE";
        break;
    case types::powermeter::OCMFIdentificationFlags::OCPP_RS:
        result = "OCPP_RS";
        break;
    case types::powermeter::OCMFIdentificationFlags::OCPP_AUTH:
        result = "OCPP_AUTH";
        break;
    case types::powermeter::OCMFIdentificationFlags::OCPP_RS_TLS:
        result = "OCPP_RS_TLS";
        break;
    case types::powermeter::OCMFIdentificationFlags::OCPP_AUTH_TLS:
        result = "OCPP_AUTH_TLS";
        break;
    case types::powermeter::OCMFIdentificationFlags::OCPP_CACHE:
        result = "OCPP_CACHE";
        break;
    case types::powermeter::OCMFIdentificationFlags::OCPP_WHITELIST:
        result = "OCPP_WHITELIST";
        break;
    case types::powermeter::OCMFIdentificationFlags::OCPP_CERTIFIED:
        result = "OCPP_CERTIFIED";
        break;
    case types::powermeter::OCMFIdentificationFlags::ISO15118_NONE:
        result = "ISO15118_NONE";
        break;
    case types::powermeter::OCMFIdentificationFlags::ISO15118_PNC:
        result = "ISO15118_PNC";
        break;
    case types::powermeter::OCMFIdentificationFlags::PLMN_NONE:
        result = "PLMN_NONE";
        break;
    case types::powermeter::OCMFIdentificationFlags::PLMN_RING:
        result = "PLMN_RING";
        break;
    case types::powermeter::OCMFIdentificationFlags::PLMN_SMS:
        result = "PLMN_SMS";
        break;
    default:
        result = "UNKNOWN";
        break;
    }
    return result;
}

std::string
IsaIemDcrController::helper_get_string_from_OCMFIdentificationType(types::powermeter::OCMFIdentificationType IT) {
    std::string result;
    switch (IT) {
    case types::powermeter::OCMFIdentificationType::DENIED:
        result = "DENIED";
        break;
    case types::powermeter::OCMFIdentificationType::UNDEFINED:
        result = "UNDEFINED";
        break;
    case types::powermeter::OCMFIdentificationType::ISO14443:
        result = "ISO14443";
        break;
    case types::powermeter::OCMFIdentificationType::ISO15693:
        result = "ISO15693";
        break;
    case types::powermeter::OCMFIdentificationType::EMAID:
        result = "EMAID";
        break;
    case types::powermeter::OCMFIdentificationType::EVCCID:
        result = "EVCCID";
        break;
    case types::powermeter::OCMFIdentificationType::EVCOID:
        result = "EVCOID";
        break;
    case types::powermeter::OCMFIdentificationType::ISO7812:
        result = "ISO7812";
        break;
    case types::powermeter::OCMFIdentificationType::CARD_TXN_NR:
        result = "CARD_TXN_NR";
        break;
    case types::powermeter::OCMFIdentificationType::CENTRAL:
        result = "CENTRAL";
        break;
    case types::powermeter::OCMFIdentificationType::CENTRAL_1:
        result = "CENTRAL_1";
        break;
    case types::powermeter::OCMFIdentificationType::CENTRAL_2:
        result = "CENTRAL_2";
        break;
    case types::powermeter::OCMFIdentificationType::LOCAL:
        result = "LOCAL";
        break;
    case types::powermeter::OCMFIdentificationType::LOCAL_1:
        result = "LOCAL_1";
        break;
    case types::powermeter::OCMFIdentificationType::LOCAL_2:
        result = "LOCAL_2";
        break;
    case types::powermeter::OCMFIdentificationType::PHONE_NUMBER:
        result = "PHONE_NUMBER";
        break;
    case types::powermeter::OCMFIdentificationType::KEY_CODE:
        result = "KEY_CODE";
        break;
    default:
        result = "NONE";
        break;
    }
    return result;
}

std::chrono::minutes IsaIemDcrController::helper_convert_timezone(std::string& timezone) {
    const char sign_char = timezone[0];
    const int offset_hours = std::stoi(timezone.substr(1, 2));
    const int offset_minutes = std::stoi(timezone.substr(3, 2));
    const std::chrono::minutes time_offset = std::chrono::hours(offset_hours) + std::chrono::minutes(offset_minutes);
    if (sign_char == '+') {
        return time_offset;
    } else {
        return -time_offset;
    }
}

bool IsaIemDcrController::helper_is_daylight_saving_time() {
    const std::time_t now = std::time(nullptr);
    const std::tm* localTime = std::localtime(&now);
    return localTime->tm_isdst > 0;
}

std::string IsaIemDcrController::helper_get_current_datetime() {
    // Get UTC time
    auto now = std::chrono::system_clock::now();
    // Add configured timezone information
    std::time_t now_with_offset = std::chrono::system_clock::to_time_t(now + zone_time_offset);
    // Add DST offset if configured
    if (snapshot_config.timezone_handle_DST && helper_is_daylight_saving_time()) {
        now_with_offset = now_with_offset + 3600;
    }
    // Generate and return time in correct format
    std::ostringstream oss;
    oss << std::put_time(gmtime(&now_with_offset), "%FT%T,000") << snapshot_config.timezone;
    return oss.str();
}

std::string IsaIemDcrController::helper_remove_first_and_last_char(const std::string& input) {
    if (input.length() <= 1) {
        return "";
    }
    return input.substr(1, input.length() - 1);
}

types::units_signed::SignedMeterValue IsaIemDcrController::helper_get_signed_datatuple(const std::string& endpoint) {
    auto response = this->http_client->get(endpoint);
    types::units_signed::SignedMeterValue return_value;
    if (response.status_code == 200) {
        try {
            return_value.signed_meter_data = response.body;
            return_value.signing_method = "";
            return_value.encoding_method = "OCMF";
            return_value.public_key = get_publickey(true);

            return return_value;
        } catch (json::exception& json_error) {
            throw UnexpectedIemDcrResponseBody(
                endpoint, fmt::format("Json error {} for body {}", json_error.what(), response.body));
        }
    } else {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

} // namespace module::main
