// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"
#include "http_client.hpp"
#include "lem_dcbm_time_sync_helper.hpp"
#include <chrono>
#include <string>
#include <thread>

namespace module::main {

void powermeterImpl::init() {
    // Dependency injection pattern: Create the HTTP client first,
    // then move it into the controller as a constructor argument
    auto http_client =
        std::make_unique<HttpClient>(mod->config.ip_address, mod->config.port, mod->config.meter_tls_certificate);

    auto ntp_server_spec =
        module::main::ntp_server_spec{mod->config.ntp_server_1_ip_addr, mod->config.ntp_server_1_port,
                                      mod->config.ntp_server_2_ip_addr, mod->config.ntp_server_2_port};

    this->controller = std::make_unique<LemDCBM400600Controller>(
        std::move(http_client), std::make_unique<LemDCBMTimeSyncHelper>(ntp_server_spec),
        LemDCBM400600Controller::Conf{
            mod->config.resilience_initial_connection_retries, mod->config.resilience_initial_connection_retry_delay,
            mod->config.resilience_transaction_request_retries, mod->config.resilience_transaction_request_retry_delay,
            mod->config.cable_id, mod->config.tariff_id, mod->config.meter_timezone, mod->config.meter_dst,
            mod->config.SC, mod->config.UV, mod->config.UD});

    this->controller->init();
}

void powermeterImpl::ready() {
    // Start the live_measure_publisher thread, which periodically publishes the live measurements of the device
    this->live_measure_publisher_thread = std::thread([this] {
        this->publish_public_key_ocmf(this->controller->get_public_key_ocmf());
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            try {
                this->publish_powermeter(this->controller->get_powermeter());
            } catch (LemDCBM400600Controller::DCBMUnexpectedResponseException& dcbm_exception) {
                EVLOG_error << "Failed to publish powermeter value due to an invalid device response: "
                            << dcbm_exception.what();
            } catch (HttpClientError& client_error) {
                EVLOG_error << "Failed to publish powermeter value due to an http error: " << client_error.what();
            }
        }
    });
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    return this->controller->start_transaction(value);
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    return this->controller->stop_transaction(transaction_id);
}

} // namespace module::main
