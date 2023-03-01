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
}

void isolation_monitorImpl::ready() {
}

isolation_monitorImpl::~isolation_monitorImpl() {
}

void isolation_monitorImpl::handle_start() {
    if (this->isolation_monitoring_active == false) {
        this->isolation_monitoring_active = true;
        EVLOG_info << "Started simulated isolation monitoring with " << this->config_interval << " ms interval";
    }
};

void isolation_monitorImpl::isolation_measurement_worker() {
    while (true) {
        if (this->isolation_measurement_thread_handle.shouldExit()) {
            break;
        }

        if (this->isolation_monitoring_active == true) {
            this->mod->p_main->publish_IsolationMeasurement(this->isolation_measurement);
            EVLOG_debug << "Simulated isolation measurement finished";
            std::this_thread::sleep_for(std::chrono::milliseconds(this->config_interval - this->LOOP_SLEEP_MS));
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
