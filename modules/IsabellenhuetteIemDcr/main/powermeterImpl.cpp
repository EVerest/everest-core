// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"
#include "httpClient.hpp"
#include <chrono>
#include <string>
#include <thread>

namespace module {
namespace main {

void powermeterImpl::init() {
    // Check Config values (essential plausibility checks)
    check_config();

    // Dependency injection pattern: Create the HTTP client first,
    // then move it into the controller as a constructor argument
    auto http_client = std::make_unique<HttpClient>(mod->config.ip_address, mod->config.port_http);

    // Create controller object
    this->controller = std::make_unique<IsaIemDcrController>(
        std::move(http_client),
        IsaIemDcrController::SnapshotConfig{
            mod->config.timezone, mod->config.timezone_handle_DST, mod->config.datetime_resync_interval,
            mod->config.resilience_initial_connection_retry_delay, mod->config.resilience_transaction_request_retries,
            mod->config.resilience_transaction_request_retry_delay, mod->config.CT, mod->config.CI,
            mod->config.TT_initial, mod->config.US});
}

void powermeterImpl::ready() {
    // Start the live_measure_publisher thread, which periodically publishes the live measurements of the device
    this->live_measure_publisher_thread = std::thread([this] {
        while (true) {
            try {
                // Wait for at least one second (more if handle_start_transaction() or handle_stop_transaction() active)
                do {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                } while (start_transaction_running.load() == true || stop_transaction_running.load() == true);
                // Init if needed
                if (is_initialized.load() == false) {
                    is_initialized.store(this->controller->init());
                    if (is_initialized.load() == true) {
                        // Publish public key once after init
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        this->publish_public_key_ocmf(this->controller->get_publickey(false));
                    } else {
                        if (!this->error_state_monitor->is_error_active("powermeter/CommunicationFault",
                                                                        "Communication timed out")) {
                            auto error = this->error_factory->create_error(
                                "powermeter/CommunicationFault", "Communication timed out",
                                "This error is raised due to communication timeout");
                            raise_error(error);
                        }
                        EVLOG_warning << "Connecting to IEM-DCR failed. Retry in "
                                      << mod->config.resilience_initial_connection_retry_delay << " milliseconds";
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(mod->config.resilience_initial_connection_retry_delay));
                    }
                } else {
                    // Publish metervalue node (named powermeter in EVerest) and update status information
                    auto meter_value_response = this->controller->get_metervalue();
                    types::powermeter::Powermeter tmp_powermeter;
                    std::string tmp_status;
                    bool tmp_transaction_active;
                    std::tie(tmp_powermeter, tmp_status, tmp_transaction_active) = meter_value_response;
                    this->publish_powermeter(tmp_powermeter);
                    dcr_status.store(tmp_status);
                    transaction_active.store(tmp_transaction_active);

                    // Debug output :)
                    // EVLOG_info << this->controller->get_datetime();

                    // Update datetime in specified interval
                    if (transaction_active.load() == false) {
                        this->controller->refresh_datetime_if_required();
                    }

                    // Reset previous error (if active)
                    if (this->error_state_monitor->is_error_active("powermeter/CommunicationFault",
                                                                   "Communication timed out")) {
                        clear_error("powermeter/CommunicationFault", "Communication timed out");
                    }
                }
            } catch (HttpClientError& client_error) {
                is_initialized.store(false);
                if (!this->error_state_monitor->is_error_active("powermeter/CommunicationFault",
                                                                "Communication timed out")) {
                    EVLOG_error << "Failed to communicate with the powermeter due to http error: "
                                << client_error.what();
                    auto error =
                        this->error_factory->create_error("powermeter/CommunicationFault", "Communication timed out",
                                                          "This error is raised due to communication timeout");
                    raise_error(error);
                }
            } catch (const std::exception& e) {
                is_initialized.store(false);
                EVLOG_error << "Exception in cyclic IEM-DCR communication: " << e.what();
            }
        }
    });
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    // your code for cmd start_transaction goes here
    types::powermeter::TransactionStartResponse return_value;

    start_transaction_running.store(true);

    // Check preconditions
    if (value.evse_id != mod->config.CI && value.evse_id.length() > 0) {
        return_value.status = types::powermeter::TransactionRequestStatus::NOT_SUPPORTED;
        return_value.error = "config: CI does not match evse_id. This is not allowed.";
        EVLOG_error << "Aborted: " << *return_value.error;
        return return_value;
    }
    if (is_initialized.load() == false) {
        return_value.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
        return_value.error = "Init of communication not finished yet. Please try again later.";
        EVLOG_error << "Aborted: " << *return_value.error;
        return return_value;
    }
    if (dcr_status.load() != "0x0000, 0x00000000, 0x00, 0x00") {
        return_value.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
        return_value.error = "IEM-DCR is in error state. XC: " + dcr_status.load();
        EVLOG_error << "Aborted: " << *return_value.error;
        return return_value;
    }

    // Perform action
    try {
        // Check if gw information is already set
        if (this->controller->check_gw_is_empty()) {
            return_value.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
            return_value.error = "Init seems to be missing. Re-Init triggered. Please try again later.";
            is_initialized.store(false);
        } else {
            // Stop transaction (if a transaction is still running)
            if (transaction_active.load() == true) {
                this->controller->call_with_retry([this]() { this->controller->post_receipt("E"); },
                                                  mod->config.resilience_transaction_request_retries,
                                                  mod->config.resilience_transaction_request_retry_delay);
            } else {
                // Try to end transaction at least once.
                try {
                    this->controller->post_receipt("E");
                } catch (...) {
                    // Nothing to do here
                }
            }
            // Wait for being surely in idle mode
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            // Create user
            if ((static_cast<std::string>(value.identification_data.value_or(""))).length() <= 0) {
                this->controller->call_with_retry(
                    [this, value]() {
                        this->controller->post_user(value.identification_status, value.identification_level,
                                                    value.identification_flags, value.identification_type,
                                                    value.transaction_id, value.tariff_text);
                    },
                    mod->config.resilience_transaction_request_retries,
                    mod->config.resilience_transaction_request_retry_delay);
            } else {
                this->controller->call_with_retry(
                    [this, value]() {
                        this->controller->post_user(value.identification_status, value.identification_level,
                                                    value.identification_flags, value.identification_type,
                                                    value.identification_data, value.tariff_text);
                    },
                    mod->config.resilience_transaction_request_retries,
                    mod->config.resilience_transaction_request_retry_delay);
            }
            // Start transaction
            this->controller->call_with_retry([this]() { this->controller->post_receipt("B"); },
                                              mod->config.resilience_transaction_request_retries,
                                              mod->config.resilience_transaction_request_retry_delay);
            // Prepare positive response
            transaction_active.store(true);
            return_value.status = types::powermeter::TransactionRequestStatus::OK;
            return_value.error = "";
        }
    } catch (const std::exception& e) {
        return_value.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
        return_value.error = e.what();
        EVLOG_error << "Aborted: " << return_value.error.value_or("");
    }

    start_transaction_running.store(false);

    return return_value;
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    // your code for cmd stop_transaction goes here
    types::powermeter::TransactionStopResponse return_value;

    stop_transaction_running.store(true);

    if (is_initialized.load() == false) {
        return_value.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
        return_value.error = "Init of communication not finished yet.";
        EVLOG_error << "Aborted: " << *return_value.error;
    } else if (transaction_active.load() == true) {
        try {
            // Stop transaction
            this->controller->call_with_retry([this]() { this->controller->post_receipt("E"); },
                                              mod->config.resilience_transaction_request_retries,
                                              mod->config.resilience_transaction_request_retry_delay);
            // Wait for signature calculation
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            // Read receipt
            return_value.signed_meter_value =
                this->controller->call_with_retry([this]() { return this->controller->get_receipt(); },
                                                  mod->config.resilience_transaction_request_retries,
                                                  mod->config.resilience_transaction_request_retry_delay);
            // Prepare positive response
            transaction_active.store(false);
            return_value.status = types::powermeter::TransactionRequestStatus::OK;
            return_value.error = "";
        } catch (const std::exception& e) {
            return_value.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
            return_value.error = e.what();
            EVLOG_error << "Aborted: " << return_value.error.value_or("");
        }
    } else {
        // No transaction running. So return last transaction (if available)
        try {
            return_value.signed_meter_value = this->controller->get_transaction();
            return_value.status = types::powermeter::TransactionRequestStatus::OK;
            return_value.error = "";
        } catch (std::exception& e) {
            return_value.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
            return_value.error = std::string(e.what()) + " Maybe no transaction to return?";
            EVLOG_warning << "Aborted: " << return_value.error.value_or("");
        }
    }

    stop_transaction_running.store(false);

    return return_value;
}

void powermeterImpl::check_config() {
    // Numeric range checks are aready covered by manifest minimum and maximum declaration
    if (mod->config.ip_address.length() <= 0) {
        EVLOG_error << "Incorrect module config: parameter ip_address is empty." << std::endl;
        throw std::runtime_error("ip_address invalid. Please check configuration.");
    }
    if (mod->config.timezone.length() != 5) {
        EVLOG_error
            << "Incorrect module config: parameter timezone has invalid length. 5 characters expected, e.g. +0200"
            << std::endl;
        throw std::runtime_error("Timezone invalid. Please check configuration.");
    }
    if (mod->config.timezone[0] != '+' && mod->config.timezone[0] != '-') {
        EVLOG_error << "Incorrect module config: parameter timezone has invalid format. It must begin with + or - char."
                    << std::endl;
        throw std::runtime_error("Timezone invalid. Please check configuration.");
    }
    if (mod->config.CT.length() <= 0) {
        EVLOG_error << "Incorrect module config: parameter CT is empty." << std::endl;
        throw std::runtime_error("CT invalid. Please check configuration.");
    }
    if (mod->config.CI.length() <= 0) {
        EVLOG_error << "Incorrect module config: parameter CI is empty." << std::endl;
        throw std::runtime_error("CI invalid. Please check configuration.");
    }
}

} // namespace main
} // namespace module
