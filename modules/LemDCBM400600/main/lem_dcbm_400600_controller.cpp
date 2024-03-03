// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "lem_dcbm_400600_controller.hpp"
#include <stdexcept>
namespace module::main {

void LemDCBM400600Controller::init() {
    try {
        call_with_retry([this]() { this->fetch_meter_id_from_device(); }, this->config.init_number_of_http_retries,
                        this->config.init_retry_wait_in_milliseconds);
    } catch (HttpClientError& http_client_error) {
        EVLOG_error << "Initialization of LemDCBM400600Controller failed with http client error: "
                    << http_client_error.what();
        throw;
    } catch (DCBMUnexpectedResponseException& dcbm_error) {
        EVLOG_error << "Initialization of LemDCBM400600Controller failed due an unexpected device response: "
                    << dcbm_error.what();
        throw;
    }

    this->time_sync_helper->restart_unsafe_period();
}

void LemDCBM400600Controller::fetch_meter_id_from_device() {
    auto status_response = this->http_client->get("/v1/status");

    if (status_response.status_code != 200) {
        throw UnexpectedDCBMResponseCode("/v1/status", 200, status_response);
    }
    try {
        this->meter_id = json::parse(status_response.body).at("meterId");
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

    auto response = this->http_client->post(
        "/v1/legal", module::main::LemDCBM400600Controller::transaction_start_request_to_dcbm_payload(value));
    if (response.status_code != 201) {
        throw UnexpectedDCBMResponseCode("/v1/legal", 201, response);
    }
    try {
        bool running = json::parse(response.body).at("running");
        if (!running) {
            throw UnexpectedDCBMResponseBody(
                "/v1/legal", fmt::format("Created transaction {} has state running = false.", value.transaction_id));
        }
    } catch (json::exception& json_error) {
        throw UnexpectedDCBMResponseBody("/v1/legal",
                                         fmt::format("Json error {} for body '{}'", json_error.what(), response.body));
    }
}

types::powermeter::TransactionStopResponse
LemDCBM400600Controller::stop_transaction(const std::string& transaction_id) {
    try {
        return call_with_retry(
            [this, transaction_id]() {
                this->request_device_to_stop_transaction(transaction_id);
                auto signed_meter_value =
                    types::units_signed::SignedMeterValue{fetch_ocmf_result(transaction_id), "", "OCMF"};
                return types::powermeter::TransactionStopResponse{types::powermeter::TransactionRequestStatus::OK,
                                                                  {}, // Empty start_signed_meter_value
                                                                  signed_meter_value};
            },
            this->config.transaction_number_of_http_retries, this->config.transaction_retry_wait_in_milliseconds);
    } catch (DCBMUnexpectedResponseException& error) {
        std::string error_message = fmt::format("Failed to stop transaction {}: {}", transaction_id, error.what());
        EVLOG_error << error_message;
        return types::powermeter::TransactionStopResponse{
            types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, error_message};
    } catch (HttpClientError& error) {
        std::string error_message = fmt::format("Failed to stop transaction {} - connection to device failed: {}",
                                                transaction_id, error.what());
        EVLOG_error << error_message;
        return types::powermeter::TransactionStopResponse{
            types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, error_message};
    }
}

void LemDCBM400600Controller::request_device_to_stop_transaction(const std::string& transaction_id) {
    std::string endpoint = fmt::format("/v1/legal?transactionId={}", transaction_id);
    auto legal_api_response = this->http_client->put(endpoint, R"({"running": false})");

    if (legal_api_response.status_code != 200) {
        throw UnexpectedDCBMResponseCode(endpoint, 200, legal_api_response);
    }

    try {
        int status = json::parse(legal_api_response.body).at("meterValue").at("transactionStatus");
        bool transaction_is_ongoing = (status & 0b100) != 0; //  third status bit "transactionIsOnGoing" must be false
        if (transaction_is_ongoing) {
            throw UnexpectedDCBMResponseBody(
                endpoint, fmt::format("Transaction stop request for transaction {} returned device status {}.",
                                      transaction_id, status));
        }
    } catch (json::exception& json_error) {
        throw UnexpectedDCBMResponseBody(
            endpoint, fmt::format("Json error '{}' for body {}", json_error.what(), legal_api_response.body));
    }
}

std::string LemDCBM400600Controller::fetch_ocmf_result(const std::string& transaction_id) {
    const std::string ocmf_endpoint = fmt::format("/v1/ocmf?transactionId={}", transaction_id);
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

    auto response = this->http_client->get("/v1/livemeasure");
    if (response.status_code != 200) {
        throw UnexpectedDCBMResponseCode("/v1/livemeasure", 200, response);
    }
    types::powermeter::Powermeter value;
    try {
        this->convert_livemeasure_to_powermeter(response.body, value);
        return value;
    } catch (json::exception& json_error) {
        throw UnexpectedDCBMResponseBody("/v1/livemeasure", fmt::format("Json error '{}'", json_error.what()));
    }
}

void LemDCBM400600Controller::convert_livemeasure_to_powermeter(const std::string& livemeasure,
                                                                types::powermeter::Powermeter& powermeter) {
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
}
std::string
LemDCBM400600Controller::transaction_start_request_to_dcbm_payload(const types::powermeter::TransactionReq& request) {
    return nlohmann::ordered_json{{"evseId", request.evse_id},     {"transactionId", request.transaction_id},
                                  {"clientId", request.client_id}, {"tariffId", request.tariff_id},
                                  {"cableId", request.cable_id},   {"userData", request.user_data}}
        .dump();
}

std::pair<std::string, std::string> LemDCBM400600Controller::get_transaction_stop_time_bounds() {
    // The LEM DCBM 400/600 Operations manual (7.2.2.) states
    // "Minimum duration for transactions is 2 minutes, to prevent potential memory storage weaknesses."
    // Further, the communication protocol states (4.2.9.):
    // "If after a period of 48h the time was not set, time synchronization expires (preventing new transactions and
    // invalidating on-going one)."" Since during an ongoing transaction, now time can synced, the max duration is set
    // to 48 hours (minus a small delta).
    auto now = std::chrono::time_point<date::utc_clock>::clock::now();
    return {
        Everest::Date::to_rfc3339(now + std::chrono::minutes(2)),
        Everest::Date::to_rfc3339(now + std::chrono::hours(48) - std::chrono::minutes(1)),
    };
}

} // namespace module::main
