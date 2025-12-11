// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "over_voltage_monitorImpl.hpp"

namespace module {
namespace main {

void over_voltage_monitorImpl::init() {
    over_voltage_monitoring_active = false;
    over_voltage_monitorImpl_thread_handle =
        std::thread(&over_voltage_monitorImpl::over_voltage_monitorImpl_worker, this);
    const std::string OVER_VOLTAGE_TOPIC = "everest_api/" + this->mod->info.id + "/cmd/set_over_voltage_measurement_V";
    EVLOG_info << "Can control the over voltage measurement values over the topic: " << OVER_VOLTAGE_TOPIC
               << " via MQTT";
    this->mod->mqtt.subscribe(OVER_VOLTAGE_TOPIC, [this](const std::string& over_voltage_measurement_V) {
        try {
            EVLOG_info << "Setting simulated over voltage measurement to " << over_voltage_measurement_V
                       << " V via MQTT";
            voltage_measurement_V = std::stof(over_voltage_measurement_V);
        } catch (const std::invalid_argument& e) {
            EVLOG_error << "Failed to set over voltage measurement via MQTT, invalid value: "
                        << over_voltage_measurement_V;
        }
    });
}

void over_voltage_monitorImpl::ready() {
}

void over_voltage_monitorImpl::handle_set_limits(double& emergency_over_voltage_limit_V,
                                                 double& error_over_voltage_limit_V) {
    error_threshold = error_over_voltage_limit_V;
    emergency_threshold = emergency_over_voltage_limit_V;
}

void over_voltage_monitorImpl::handle_start() {
    EVLOG_info << "Over voltage monitoring: starting with " << emergency_threshold << " (emergency) and "
               << error_threshold << "(error)";
    over_voltage_monitoring_active = true;
    if (config.simulate_error_shutdown) {
        std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(config.simulate_error_delay));
            auto err = error_factory->create_error("over_voltage_monitor/MREC5OverVoltage", "",
                                                   "Simulated Over voltage error shutdown occurred.",
                                                   Everest::error::Severity::Medium);
            raise_error(err);
        }).detach();
    }

    if (config.simulate_emergency_shutdown) {
        std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(config.simulate_error_delay));
            auto err = error_factory->create_error("over_voltage_monitor/MREC5OverVoltage", "",
                                                   "Simulated Over voltage emergency shutdown occurred.",
                                                   Everest::error::Severity::High);
            raise_error(err);
        }).detach();
    }
}

void over_voltage_monitorImpl::over_voltage_monitorImpl_worker() {
    while (true) {
        if (this->over_voltage_monitorImpl_thread_handle.shouldExit()) {
            break;
        }

        if (this->over_voltage_monitoring_active == true) {
            this->mod->p_main->publish_voltage_measurement_V(this->voltage_measurement_V);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->LOOP_SLEEP_MS));
    }
}

void over_voltage_monitorImpl::handle_stop() {
    EVLOG_info << "Over voltage monitoring: stopped.";
    over_voltage_monitoring_active = false;
}

void over_voltage_monitorImpl::handle_reset_over_voltage_error() {
    EVLOG_info << "Over voltage monitoring: reset";
    clear_all_errors_of_impl("over_voltage_monitor/MREC5OverVoltage");
}

} // namespace main
} // namespace module
