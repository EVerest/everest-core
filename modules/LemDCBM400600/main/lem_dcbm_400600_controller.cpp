// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "lem_dcbm_400600_controller.hpp"
#include <stdexcept>
namespace module::main {

void LemDCBM400600Controller::init() {
    try {
        this->time_sync_helper->set_time_config_params(config.meter_timezone, config.meter_dst);
        call_with_retry([this]() { this->fetch_meter_id_from_device(); }, this->config.init_number_of_http_retries,
                        this->config.init_retry_wait_in_milliseconds);
    } catch (HttpClientError& http_client_error) {
        EVLOG_error << "Initialization of LemDCBM400600Controller failed with http "
                       "client error: "
                    << http_client_error.what();
        throw;
    } catch (DCBMUnexpectedResponseException& dcbm_error) {
        EVLOG_error << "Initialization of LemDCBM400600Controller failed due an "
                       "unexpected device response: "
                    << dcbm_error.what();
        throw;
    }

    this->time_sync_helper->restart_unsafe_period();
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string LemDCBM400600Controller::get_current_transaction() {
    const std::string endpoint = v2_capable ? "/v2/legal" : "/v1/legal";
    auto response = this->http_client->get(endpoint);
    if (response.status_code != 200) {
        throw UnexpectedDCBMResponseCode(endpoint, 200, response);
    }
    try {
        json data = json::parse(response.body);
        return data.at("transactionId");
    } catch (json::exception& json_error) {
        throw UnexpectedDCBMResponseBody(endpoint, fmt::format("Json error '{}'", json_error.what()));
    }
}

void LemDCBM400600Controller::fetch_meter_id_from_device() {
    auto status_response = this->http_client->get("/v1/status");

    if (status_response.status_code != 200) {
        throw UnexpectedDCBMResponseCode("/v1/status", 200, status_response);
    }
    try {
        json data = json::parse(status_response.body);
        this->meter_id = data.at("meterId");
        this->public_key_ocmf = data.at("publicKeyOcmf");
        this->transaction_is_ongoing_at_startup = data.at("status").at("bits").at("transactionIsOnGoing");
        std::string version = data.at("version").at("applicationFirmwareVersion");
        auto components = split(version, '.');
        this->v2_capable =
            ((components.size() == 4) && (components[1] > "1")); // the major version must be newer than 1
        this->current_transaction_id = get_current_transaction();
        if (this->transaction_is_ongoing_at_startup) {
            // we need to get the current transaction id or the last known transaction id since we might
            // receive a stop transaction with id 0 or with the last known transaction if
            // meaning that the system had a failure and the transaction was started but id was not saved
            // and we try to recover from the error, thus we need to cancel the transaction
            EVLOG_warning << "LEM DCBM 400/600: A transaction is already ongoing and it has the id:"
                          << this->current_transaction_id;
        } else {
            EVLOG_info << "LEM DCBM 400/600: The last known transaction has the id:" << this->current_transaction_id;
        }
    } catch (json::exception& json_error) {
        throw UnexpectedDCBMResponseBody(
            "/v1/status", fmt::format("Json error {} for body {}", json_error.what(), status_response.body));
    }
}

types::powermeter::TransactionStartResponse
LemDCBM400600Controller::start_transaction(const types::powermeter::TransactionReq& value) {
    try {
        call_with_retry([this, value]() { this->request_device_to_start_transaction(value); },
                        this->config.transaction_number_of_http_retries,
                        this->config.transaction_retry_wait_in_milliseconds);
    } catch (DCBMUnexpectedResponseException& error) {
        const std::string error_message =
            fmt::format("Failed to start transaction {}: {}", value.transaction_id, error.what());
        EVLOG_error << error_message;
        return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, error_message};
    } catch (HttpClientError& error) {
        const std::string error_message = fmt::format(
            "Failed to start transaction {} - connection to device failed: {}", value.transaction_id, error.what());
        EVLOG_error << error_message;
        return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, error_message};
    }

    auto [transaction_min_stop_time, transaction_max_stop_time] = get_transaction_stop_time_bounds();

    return {types::powermeter::TransactionRequestStatus::OK, {}, transaction_min_stop_time, transaction_max_stop_time};
}

void LemDCBM400600Controller::request_device_to_start_transaction(const types::powermeter::TransactionReq& value) {
    this->time_sync_helper->sync(*this->http_client);

    const std::string endpoint = v2_capable ? "/v2/legal" : "/v1/legal";
    const std::string payload = this->transaction_start_request_to_dcbm_payload(value);
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 201) {
        throw UnexpectedDCBMResponseCode(endpoint, 201, response);
    }
    try {
        bool running = json::parse(response.body).at("running");
        if (!running) {
            throw UnexpectedDCBMResponseBody(
                "/v1/legal", fmt::format("Created transaction {} has state running = false.", value.transaction_id));
        }
    } catch (json::exception& json_error) {
        throw UnexpectedDCBMResponseBody(endpoint,
                                         fmt::format("Json error {} for body '{}'", json_error.what(), response.body));
    }
}

types::powermeter::TransactionStopResponse
LemDCBM400600Controller::stop_transaction(const std::string& transaction_id) {
    std::string tid = transaction_id;
    bool need_to_stop_the_transaction = true;
    if (!(this->transaction_is_ongoing_at_startup) && transaction_id == this->current_transaction_id) {
        // transaction is not open but we need to provide OCMF information about it
        need_to_stop_the_transaction = false;
        this->current_transaction_id = "";
    }
    if (!(this->transaction_is_ongoing_at_startup) && transaction_id.empty()) {
        // return an error because there is no transaction initially ongoing (at start up time)
        return types::powermeter::TransactionStopResponse{
            types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, "No dangling transaction open"};
    }
    if (this->transaction_is_ongoing_at_startup && transaction_id == this->current_transaction_id) {
        // transaction has been found and it is now going to close
        this->transaction_is_ongoing_at_startup = false;
        this->current_transaction_id = "";
    }
    if (this->transaction_is_ongoing_at_startup && transaction_id.empty()) {
        // transaction has NOT been found, we use the last known value of the transaction id
        tid = this->current_transaction_id;
        this->current_transaction_id = "";
        this->transaction_is_ongoing_at_startup = false;
    }
    try {
        return call_with_retry(
            [this, need_to_stop_the_transaction, tid]() {
                // special case if we started and a transaction is ongoing - the upper layers might not know the
                // transaction id
                if (need_to_stop_the_transaction) {
                    this->request_device_to_stop_transaction(tid);
                }
                auto signed_meter_value = types::units_signed::SignedMeterValue{fetch_ocmf_result(tid), "", "OCMF"};
                return types::powermeter::TransactionStopResponse{types::powermeter::TransactionRequestStatus::OK,
                                                                  {}, // Empty start_signed_meter_value
                                                                  signed_meter_value};
            },
            this->config.transaction_number_of_http_retries, this->config.transaction_retry_wait_in_milliseconds);
    } catch (DCBMUnexpectedResponseException& error) {
        std::string error_message = fmt::format("Failed to stop transaction {}: {}", tid, error.what());
        EVLOG_error << error_message;
        return types::powermeter::TransactionStopResponse{
            types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, error_message};
    } catch (HttpClientError& error) {
        std::string error_message =
            fmt::format("Failed to stop transaction {} - connection to device failed: {}", tid, error.what());
        EVLOG_error << error_message;
        return types::powermeter::TransactionStopResponse{
            types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, error_message};
    }
}

void LemDCBM400600Controller::request_device_to_stop_transaction(const std::string& transaction_id) {
    std::string endpoint = v2_capable ? fmt::format("/v2/legal?transactionId={}", transaction_id)
                                      : fmt::format("/v1/legal?transactionId={}", transaction_id);
    auto legal_api_response = this->http_client->put(endpoint, R"({"running": false})");

    if (legal_api_response.status_code != 200) {
        throw UnexpectedDCBMResponseCode(endpoint, 200, legal_api_response);
    }

    try {
        int status = json::parse(legal_api_response.body).at("meterValue").at("transactionStatus");
        bool transaction_is_ongoing = (status & 0b100) != 0; //  third status bit "transactionIsOnGoing" must be false
        if (transaction_is_ongoing) {
            throw UnexpectedDCBMResponseBody(endpoint, fmt::format("Transaction stop request for transaction {} "
                                                                   "returned device status {}.",
                                                                   transaction_id, status));
        }
    } catch (json::exception& json_error) {
        throw UnexpectedDCBMResponseBody(
            endpoint, fmt::format("Json error '{}' for body {}", json_error.what(), legal_api_response.body));
    }
}

std::string LemDCBM400600Controller::fetch_ocmf_result(const std::string& transaction_id) {
    const std::string ocmf_endpoint = v2_capable ? fmt::format("/v2/ocmf?transactionId={}", transaction_id)
                                                 : fmt::format("/v1/ocmf?transactionId={}", transaction_id);
    auto ocmf_api_response = this->http_client->get(ocmf_endpoint);

    if (ocmf_api_response.status_code != 200) {
        throw UnexpectedDCBMResponseCode(ocmf_endpoint, 200, ocmf_api_response);
    }

    if (ocmf_api_response.body.empty()) {
        throw UnexpectedDCBMResponseBody(ocmf_endpoint, "Returned empty body");
    }

    return ocmf_api_response.body;
}

types::powermeter::Powermeter LemDCBM400600Controller::get_powermeter() {
    this->time_sync_helper->sync_if_deadline_expired(*this->http_client);

    const std::string endpoint = v2_capable ? "/v2/livemeasure" : "/v1/livemeasure";
    auto response = this->http_client->get(endpoint);
    if (response.status_code != 200) {
        throw UnexpectedDCBMResponseCode(endpoint, 200, response);
    }
    try {
        return this->convert_livemeasure_to_powermeter(response.body);
    } catch (json::exception& json_error) {
        throw UnexpectedDCBMResponseBody(endpoint, fmt::format("Json error '{}'", json_error.what()));
    }
}

types::powermeter::Powermeter
LemDCBM400600Controller::convert_livemeasure_to_powermeter(const std::string& livemeasure) {
    types::powermeter::Powermeter powermeter;
    json data = json::parse(livemeasure);
    powermeter.timestamp = data.at("timestamp");
    powermeter.meter_id.emplace(this->meter_id);
    powermeter.energy_Wh_import = {data.at("energyImportTotal")};
    powermeter.energy_Wh_export.emplace(types::units::Energy{data.at("energyExportTotal")});
    auto voltage = types::units::Voltage{};
    voltage.DC = data.at("voltage");
    powermeter.voltage_V.emplace(voltage);
    auto current = types::units::Current{};
    current.DC = data.at("current");
    powermeter.current_A.emplace(current);
    powermeter.power_W.emplace(types::units::Power{data.at("power")});
    powermeter.temperatures.emplace({types::temperature::Temperature{data.at("temperatureH"), "temperatureH"},
                                     types::temperature::Temperature{data.at("temperatureL"), "temperatureL"}});
    return powermeter;
}

std::string
LemDCBM400600Controller::transaction_start_request_to_dcbm_payload(const types::powermeter::TransactionReq& request) {
    std::string client_id = request.identification_data.value_or("") + ',' + request.transaction_id;
    const int max_length_client_id = 37; // as defined by LEM documentation
    client_id = (client_id.length() > max_length_client_id) ? client_id.substr(0, max_length_client_id) : client_id;
    if (this->v2_capable) {
        return nlohmann::ordered_json{{"evseId", request.evse_id},
                                      {"transactionId", request.transaction_id},
                                      {"clientId", client_id},
                                      {"tariffId", this->config.tariff_id},
                                      {"TT", request.tariff_text.value_or("")},
                                      {"UV", this->config.UV},
                                      {"UD", this->config.UD},
                                      {"cableId", this->config.cable_id},
                                      {"userData", ""},
                                      {"SC", this->config.SC}}
            .dump();
    } else {
        return nlohmann::ordered_json{
            {"evseId", request.evse_id},          {"transactionId", request.transaction_id}, {"clientId", client_id},
            {"tariffId", this->config.tariff_id}, {"cableId", this->config.cable_id},        {"userData", ""}}
            .dump();
    }
}

std::pair<std::string, std::string> LemDCBM400600Controller::get_transaction_stop_time_bounds() {
    // The LEM DCBM 400/600 Operations manual (7.2.2.) states
    // "Minimum duration for transactions is 2 minutes, to prevent potential
    // memory storage weaknesses." Further, the communication protocol states
    // (4.2.9.): "If after a period of 48h the time was not set, time
    // synchronization expires (preventing new transactions and invalidating
    // on-going one)."" Since during an ongoing transaction, now time can synced,
    // the max duration is set to 48 hours (minus a small delta).
    auto now = std::chrono::time_point<date::utc_clock>::clock::now();
    return {
        Everest::Date::to_rfc3339(now + std::chrono::minutes(2)),
        Everest::Date::to_rfc3339(now + std::chrono::hours(48) - std::chrono::minutes(1)),
    };
}

} // namespace module::main
