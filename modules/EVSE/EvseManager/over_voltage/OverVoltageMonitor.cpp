// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "over_voltage/OverVoltageMonitor.hpp"

#include <algorithm>
#include <fmt/core.h>

#include <thread>

namespace module {

OverVoltageMonitor::OverVoltageMonitor(ErrorCallback callback, std::chrono::milliseconds duration) :
    error_callback_(std::move(callback)), duration_(duration) {
}

void OverVoltageMonitor::set_limits(double emergency_limit, double error_limit) {
    emergency_limit_ = emergency_limit;
    error_limit_ = error_limit;
    limits_valid_ = true;
}

void OverVoltageMonitor::start_monitor() {
    fault_latched_ = false;
    cancel_error_timer();
    running_ = true;
}

void OverVoltageMonitor::stop_monitor() {
    running_ = false;
    cancel_error_timer();
}

void OverVoltageMonitor::reset() {
    fault_latched_ = false;
    cancel_error_timer();
}

void OverVoltageMonitor::update_voltage(double voltage_v) {
    last_voltage_ = voltage_v;
    if (!running_ || fault_latched_ || !limits_valid_) {
        return;
    }

    if (voltage_v >= emergency_limit_) {
        cancel_error_timer();
        trigger_fault(FaultType::Emergency,
                      fmt::format("Voltage {:.2f} V exceeded emergency limit {:.2f} V.", voltage_v, emergency_limit_));
        return;
    }

    if (voltage_v >= error_limit_) {
        arm_error_timer(voltage_v);
    } else {
        cancel_error_timer();
    }
}

void OverVoltageMonitor::trigger_fault(FaultType type, const std::string& reason) {
    fault_latched_ = true;
    running_ = false;
    cancel_error_timer();
    if (error_callback_) {
        error_callback_(type, reason);
    }
}

void OverVoltageMonitor::arm_error_timer(double voltage_v) {
    if (duration_.count() == 0) {
        trigger_fault(FaultType::Error, fmt::format("Voltage {:.2f} V exceeded limit {:.2f} V for at least {} ms.",
                                                    voltage_v, error_limit_, duration_.count()));
        return;
    }

    uint64_t token;
    {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (error_timer_active_) {
            timer_voltage_snapshot_ = std::max(timer_voltage_snapshot_, voltage_v);
            return;
        }
        error_timer_active_ = true;
        timer_voltage_snapshot_ = voltage_v;
        token = ++timer_sequence_;
        current_timer_sequence_ = token;
    }

    std::thread([this, token]() {
        std::this_thread::sleep_for(duration_);
        double voltage = 0.0;
        {
            std::lock_guard<std::mutex> lock(timer_mutex_);
            if (!error_timer_active_ || current_timer_sequence_ != token) {
                return;
            }
            error_timer_active_ = false;
            voltage = timer_voltage_snapshot_;
        }
        trigger_fault(FaultType::Error, fmt::format("Voltage {:.2f} V exceeded limit {:.2f} V for at least {} ms.",
                                                    voltage, error_limit_, duration_.count()));
    }).detach();
}

void OverVoltageMonitor::cancel_error_timer() {
    std::lock_guard<std::mutex> lock(timer_mutex_);
    if (!error_timer_active_) {
        return;
    }
    error_timer_active_ = false;
    ++timer_sequence_;
    current_timer_sequence_ = timer_sequence_;
}

} // namespace module
