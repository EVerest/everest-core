// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "over_voltage_monitorImpl.hpp"

namespace module {
namespace main {

void over_voltage_monitorImpl::init() {
}

void over_voltage_monitorImpl::ready() {
}

void over_voltage_monitorImpl::handle_start(double& over_voltage_limit_V) {
    EVLOG_info << "Over voltage monitoring: start with " << over_voltage_limit_V;
    if (config.simulate_error) {
        std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(config.simulate_error_delay));
            auto err = error_factory->create_error("over_voltage_monitor/MREC5OverVoltage", "",
                                                   "Simulated Over voltage error occurred.");
            raise_error(err);
        }).detach();
    }
}

void over_voltage_monitorImpl::handle_stop() {
    EVLOG_info << "Over voltage monitoring: stopped.";
}

void over_voltage_monitorImpl::handle_reset_over_voltage_error() {
    EVLOG_info << "Over voltage monitoring: reset";
    clear_error("over_voltage_monitor/MREC5OverVoltage", true);
}

} // namespace main
} // namespace module
