// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "over_voltage/OverVoltageMonitor.hpp"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <fmt/core.h>
#include <thread>

namespace module {

OverVoltageMonitor::OverVoltageMonitor(ErrorCallback callback, std::chrono::milliseconds duration) :
    error_callback_(std::move(callback)), duration_(duration) {
    timer_thread_ = std::thread(&OverVoltageMonitor::timer_thread_func, this);
}

OverVoltageMonitor::~OverVoltageMonitor() {
    {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        timer_thread_exit_ = true;
        timer_armed_ = false;
    }
    timer_cv_.notify_one();
    if (timer_thread_.joinable()) {
        timer_thread_.join();
    }
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

    {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (timer_armed_) {
            timer_voltage_snapshot_ = std::max(timer_voltage_snapshot_, voltage_v);
            return;
        }
        timer_armed_ = true;
        timer_voltage_snapshot_ = voltage_v;
        timer_deadline_ = std::chrono::steady_clock::now() + duration_;
    }
    timer_cv_.notify_one();
}

void OverVoltageMonitor::cancel_error_timer() {
    {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (!timer_armed_) {
            return;
        }
        timer_armed_ = false;
    }
    timer_cv_.notify_one();
}

void OverVoltageMonitor::timer_thread_func() {
    std::unique_lock<std::mutex> lock(timer_mutex_);

    while (!timer_thread_exit_) {
        // Wait until a timer is armed or exit is requested
        timer_cv_.wait(lock, [this] { return timer_thread_exit_ || timer_armed_; });
        if (timer_thread_exit_) {
            break;
        }

        // Capture the current deadline and wait until it expires or is cancelled/updated
        auto deadline = timer_deadline_;
        while (!timer_thread_exit_ && timer_armed_) {
            if (timer_cv_.wait_until(lock, deadline) == std::cv_status::timeout) {
                break;
            }
            // Woken up: check for exit, cancellation or re-arming with a new deadline
            if (timer_thread_exit_ || !timer_armed_ || timer_deadline_ != deadline) {
                break;
            }
        }

        if (timer_thread_exit_) {
            break;
        }
        if (!timer_armed_ || timer_deadline_ != deadline) {
            // Timer was cancelled or re-armed; go back to waiting
            continue;
        }

        // Timer expired with this deadline and is still armed
        const double voltage = timer_voltage_snapshot_;
        timer_armed_ = false;

        // Release the lock while invoking the callback path
        lock.unlock();
        trigger_fault(FaultType::Error, fmt::format("Voltage {:.2f} V exceeded limit {:.2f} V for at least {} ms.",
                                                    voltage, error_limit_, duration_.count()));
        lock.lock();
    }
}

} // namespace module
