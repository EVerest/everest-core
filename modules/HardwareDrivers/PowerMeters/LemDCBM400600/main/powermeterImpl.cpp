// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"
#include "http_client.hpp"
#include "lem_dcbm_time_sync_helper.hpp"
#include <chrono>
#include <everest/logging.hpp>
#include <fmt/core.h>
#include <string>
#include <thread>

namespace module::main {

void powermeterImpl::init() {
    // Dependency injection pattern: Create the HTTP client first,
    // then move it into the controller as a constructor argument
    auto http_client = std::make_unique<HttpClient>(mod->config.ip_address, mod->config.port,
                                                    mod->config.meter_tls_certificate, mod->config.interface);

    auto ntp_server_spec =
        module::main::ntp_server_spec{mod->config.ntp_server_1_ip_addr, mod->config.ntp_server_1_port,
                                      mod->config.ntp_server_2_ip_addr, mod->config.ntp_server_2_port};

    this->controller = std::make_unique<LemDCBM400600Controller>(
        std::move(http_client), std::make_unique<LemDCBMTimeSyncHelper>(ntp_server_spec),
        LemDCBM400600Controller::Conf{
            mod->config.resilience_initial_connection_retries, mod->config.resilience_initial_connection_retry_delay,
            mod->config.resilience_transaction_request_retries, mod->config.resilience_transaction_request_retry_delay,
            mod->config.cable_id, mod->config.tariff_id, mod->config.meter_timezone, mod->config.meter_dst,
            mod->config.SC, mod->config.UV, mod->config.UD, mod->config.command_timeout_ms});

    // Validate and normalize temperature thresholds for the monitor.
    // If the error level is configured below the warning level, clamp it and log a warning.
    double warning_level_C = mod->config.temperature_warning_level_C;
    double error_level_C = mod->config.temperature_error_level_C;
    if (error_level_C < warning_level_C) {
        EVLOG_warning << "LEM DCBM 400/600: temperature_error_level_C (" << error_level_C
                      << " °C) is below temperature_warning_level_C (" << warning_level_C
                      << " °C). Clamping error level to the warning level.";
        error_level_C = warning_level_C;
    }

    this->temperature_monitor = std::make_unique<TemperatureMonitor>(
        TemperatureMonitor::Config{warning_level_C, error_level_C, mod->config.temperature_hysteresis_K,
                                   std::chrono::milliseconds(mod->config.temperature_min_time_as_valid_ms)});
}

void powermeterImpl::ready() {
    // Start the live_measure_publisher thread, which periodically publishes the live measurements of the device
    this->live_measure_publisher_thread = std::thread([this] {
        while (true) {
            try {
                if (!this->controller->is_initialized()) {
                    this->controller->init();
                    this->publish_public_key_ocmf(this->controller->get_public_key_ocmf());
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(mod->config.resilience_initial_connection_retry_delay));
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    auto powermeter_data = this->controller->get_powermeter();
                    this->publish_powermeter(powermeter_data);
                    // if the communication error is set, clear the error
                    if (this->error_state_monitor->is_error_active("powermeter/CommunicationFault",
                                                                   "Communication timed out")) {
                        // need to update LEM status since we have recovered from a communication loss
                        this->controller->update_lem_status();
                        clear_error("powermeter/CommunicationFault", "Communication timed out");
                    }

                    // Evaluate temperature thresholds
                    if (powermeter_data.temperatures.has_value() && powermeter_data.temperatures->size() >= 2) {
                        const double temp_H = powermeter_data.temperatures->at(0).temperature;
                        const double temp_L = powermeter_data.temperatures->at(1).temperature;
                        auto events = this->temperature_monitor->update(temp_H, temp_L);
                        handle_temperature_events(events, this->temperature_monitor->last_max_temperature());
                    }
                }
            } catch (LemDCBM400600Controller::DCBMUnexpectedResponseException& dcbm_exception) {
                EVLOG_error << "Failed to publish powermeter value due to an invalid device response: "
                            << dcbm_exception.what();
            } catch (HttpClientError& client_error) {
                if (!this->error_state_monitor->is_error_active("powermeter/CommunicationFault",
                                                                "Communication timed out")) {
                    EVLOG_error << "Failed to communicate with the powermeter due to http error: "
                                << client_error.what();
                    auto error =
                        this->error_factory->create_error("powermeter/CommunicationFault", "Communication timed out",
                                                          "This error is raised due to communication timeout");
                    raise_error(error);
                }
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

void powermeterImpl::handle_temperature_events(const TemperatureMonitor::Events& events, double max_temperature) {
    if (events.warning_raised) {
        EVLOG_warning << fmt::format(
            "LEM DCBM 400/600: Temperature warning raised — max temperature {:.1f} °C exceeds warning level {:.1f} °C",
            max_temperature, mod->config.temperature_warning_level_C);
        auto error =
            this->error_factory->create_error("powermeter/VendorWarning", "TemperatureWarning",
                                              fmt::format("Max temperature {:.1f} °C exceeds warning level {:.1f} °C",
                                                          max_temperature, mod->config.temperature_warning_level_C));
        raise_error(error);
    }
    if (events.warning_cleared) {
        EVLOG_info << fmt::format(
            "LEM DCBM 400/600: Temperature warning cleared — max temperature {:.1f} °C dropped below {:.1f} °C",
            max_temperature, mod->config.temperature_warning_level_C - mod->config.temperature_hysteresis_K);
        clear_error("powermeter/VendorWarning", "TemperatureWarning");
    }
    if (events.error_raised) {
        EVLOG_error << fmt::format(
            "LEM DCBM 400/600: Temperature error raised — max temperature {:.1f} °C exceeds error level {:.1f} °C",
            max_temperature, mod->config.temperature_error_level_C);
        auto error =
            this->error_factory->create_error("powermeter/VendorError", "TemperatureError",
                                              fmt::format("Max temperature {:.1f} °C exceeds error level {:.1f} °C",
                                                          max_temperature, mod->config.temperature_error_level_C));
        raise_error(error);
    }
    if (events.error_cleared) {
        EVLOG_info << fmt::format(
            "LEM DCBM 400/600: Temperature error cleared — max temperature {:.1f} °C dropped below {:.1f} °C",
            max_temperature, mod->config.temperature_error_level_C - mod->config.temperature_hysteresis_K);
        clear_error("powermeter/VendorError", "TemperatureError");
    }
}

} // namespace module::main
