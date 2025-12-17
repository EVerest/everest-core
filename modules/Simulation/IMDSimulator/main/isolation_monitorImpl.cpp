// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest
#include "isolation_monitorImpl.hpp"
#include <chrono>
#include <thread>
namespace module {

namespace main {

void isolation_monitorImpl::init() {
    this->isolation_monitoring_active = false;
    this->isolation_measurement.resistance_F_Ohm = this->config.resistance_F_Ohm;
    this->config_interval = this->config.interval;

    this->isolation_measurement_thread_handle = std::thread(&isolation_monitorImpl::isolation_measurement_worker, this);

    const std::string RESISTANCE_TOPIC = "everest_api/" + this->mod->info.id + "/cmd/set_resistance";
    this->mod->mqtt.subscribe(RESISTANCE_TOPIC, [this](const std::string& resistance_ohm) {
        try {
            int32_t _resistance_ohm = std::stoi(resistance_ohm);
            EVLOG_info << "Setting simulated isolation resistance to " << _resistance_ohm << " Ohm via MQTT";
            this->isolation_measurement.resistance_F_Ohm = _resistance_ohm;
        } catch (const std::invalid_argument& e) {
            EVLOG_error << "Failed to set isolation resistance via MQTT, invalid value: " << resistance_ohm;
        }
    });
}

void isolation_monitorImpl::ready() {
}

void isolation_monitorImpl::handle_start() {
    if (this->isolation_monitoring_active == false) {
        this->isolation_monitoring_active = true;
        EVLOG_info << "Started simulated isolation monitoring with " << this->config_interval << " ms interval";
    }
};

void isolation_monitorImpl::handle_start_self_test(double& test_voltage_V) {
    selftest_running_countdown = 3 * 1000 / LOOP_SLEEP_MS;
}

void isolation_monitorImpl::isolation_measurement_worker() {
    while (true) {
        if (this->isolation_measurement_thread_handle.shouldExit()) {
            break;
        }

        if (this->isolation_monitoring_active == true) {
            this->mod->p_main->publish_isolation_measurement(this->isolation_measurement);
            EVLOG_debug << "Simulated isolation measurement finished";
            std::this_thread::sleep_for(std::chrono::milliseconds(this->config_interval - this->LOOP_SLEEP_MS));
        }

        if (this->selftest_running_countdown > 0) {
            this->selftest_running_countdown--;
            if (this->selftest_running_countdown == 0) {
                this->mod->p_main->publish_self_test_result(config.selftest_success);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->LOOP_SLEEP_MS));
    }
}

void isolation_monitorImpl::handle_stop() {
    if (this->isolation_monitoring_active == true) {
        EVLOG_info << "Stopped simulated isolation monitoring";
        this->isolation_monitoring_active = false;
    }
};

} // namespace main
} // namespace module
