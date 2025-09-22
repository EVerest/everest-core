// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "over_voltage_monitorImpl.hpp"

namespace module {
namespace main {

void over_voltage_monitorImpl::init() {
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

void over_voltage_monitorImpl::handle_stop() {
    EVLOG_info << "Over voltage monitoring: stopped.";
}

void over_voltage_monitorImpl::handle_reset_over_voltage_error() {
    EVLOG_info << "Over voltage monitoring: reset";
    clear_all_errors_of_impl("over_voltage_monitor/MREC5OverVoltage");
}

} // namespace main
} // namespace module
