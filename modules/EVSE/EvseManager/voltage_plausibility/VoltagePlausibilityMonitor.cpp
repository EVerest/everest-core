// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "voltage_plausibility/VoltagePlausibilityMonitor.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <fmt/core.h>
#include <thread>

namespace module {

VoltagePlausibilityMonitor::VoltagePlausibilityMonitor(ErrorCallback callback, double std_deviation_threshold_V,
                                                       std::chrono::milliseconds fault_duration,
                                                       std::chrono::milliseconds measurement_max_age) :
    error_callback_(std::move(callback)),
    std_deviation_threshold_V_(std_deviation_threshold_V),
    fault_duration_(fault_duration),
    measurement_max_age_(measurement_max_age) {
    timer_thread_ = std::thread(&VoltagePlausibilityMonitor::timer_thread_func, this);
}

VoltagePlausibilityMonitor::~VoltagePlausibilityMonitor() {
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

void VoltagePlausibilityMonitor::start_monitor() {
    fault_latched_ = false;
    cancel_fault_timer();
    running_ = true;
}

void VoltagePlausibilityMonitor::stop_monitor() {
    running_ = false;
    cancel_fault_timer();
}

void VoltagePlausibilityMonitor::reset() {
    fault_latched_ = false;
    cancel_fault_timer();
}

void VoltagePlausibilityMonitor::update_power_supply_voltage(double voltage_V) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    power_supply_sample_.voltage_V = voltage_V;
    power_supply_sample_.timestamp = std::chrono::steady_clock::now();
    evaluate_voltages();
}

void VoltagePlausibilityMonitor::update_powermeter_voltage(double voltage_V) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    powermeter_sample_.voltage_V = voltage_V;
    powermeter_sample_.timestamp = std::chrono::steady_clock::now();
    evaluate_voltages();
}

void VoltagePlausibilityMonitor::update_isolation_monitor_voltage(double voltage_V) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    isolation_monitor_sample_.voltage_V = voltage_V;
    isolation_monitor_sample_.timestamp = std::chrono::steady_clock::now();
    evaluate_voltages();
}

void VoltagePlausibilityMonitor::update_over_voltage_monitor_voltage(double voltage_V) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    over_voltage_monitor_sample_.voltage_V = voltage_V;
    over_voltage_monitor_sample_.timestamp = std::chrono::steady_clock::now();
    evaluate_voltages();
}

void VoltagePlausibilityMonitor::evaluate_voltages() {
    // Check running state and fault latch (these are only modified from main thread, so no mutex needed here)
    if (!running_ || fault_latched_) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    std::vector<double> valid_voltages;
    const auto zero_time = std::chrono::steady_clock::time_point{};

    // Collect valid voltage samples (not older than max_age)
    // A timestamp of zero (default-initialized) means we've never received a value
    if (power_supply_sample_.timestamp != zero_time) {
        const auto age = std::chrono::duration_cast<std::chrono::milliseconds>(now - power_supply_sample_.timestamp);
        if (age <= measurement_max_age_) {
            valid_voltages.push_back(power_supply_sample_.voltage_V);
        }
    }

    if (powermeter_sample_.timestamp != zero_time) {
        const auto age = std::chrono::duration_cast<std::chrono::milliseconds>(now - powermeter_sample_.timestamp);
        if (age <= measurement_max_age_) {
            valid_voltages.push_back(powermeter_sample_.voltage_V);
        }
    }

    if (isolation_monitor_sample_.timestamp != zero_time) {
        const auto age =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - isolation_monitor_sample_.timestamp);
        if (age <= measurement_max_age_) {
            valid_voltages.push_back(isolation_monitor_sample_.voltage_V);
        }
    }

    if (over_voltage_monitor_sample_.timestamp != zero_time) {
        const auto age =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - over_voltage_monitor_sample_.timestamp);
        if (age <= measurement_max_age_) {
            valid_voltages.push_back(over_voltage_monitor_sample_.voltage_V);
        }
    }

    // Need at least 2 valid sources to compute standard deviation
    if (valid_voltages.size() < 2) {
        cancel_fault_timer();
        return;
    }

    // Calculate standard deviation
    const double std_deviation = calculate_standard_deviation(valid_voltages);

    // Check against threshold
    if (std_deviation > std_deviation_threshold_V_) {
        arm_fault_timer(std_deviation);
    } else {
        cancel_fault_timer();
    }
}

double VoltagePlausibilityMonitor::calculate_standard_deviation(const std::vector<double>& values) {
    if (values.size() < 2) {
        return 0.0;
    }

    // Calculate mean
    double sum = 0.0;
    for (const auto& v : values) {
        sum += v;
    }
    const double mean = sum / values.size();

    // Calculate variance using Welford's algorithm for numerical stability
    double variance = 0.0;
    for (const auto& v : values) {
        const double diff = v - mean;
        variance += diff * diff;
    }
    variance /= values.size();

    // Return standard deviation
    return std::sqrt(variance);
}

void VoltagePlausibilityMonitor::trigger_fault(const std::string& reason) {
    fault_latched_ = true;
    running_ = false;
    cancel_fault_timer();
    if (error_callback_) {
        error_callback_(reason);
    }
}

void VoltagePlausibilityMonitor::arm_fault_timer(double std_deviation_V) {
    if (fault_duration_.count() == 0) {
        trigger_fault(fmt::format("Voltage standard deviation {:.2f} V exceeded threshold {:.2f} V.", std_deviation_V,
                                  std_deviation_threshold_V_));
        return;
    }

    {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (timer_armed_) {
            timer_std_deviation_snapshot_ = std::max(timer_std_deviation_snapshot_, std_deviation_V);
            return;
        }
        timer_armed_ = true;
        timer_std_deviation_snapshot_ = std_deviation_V;
        timer_deadline_ = std::chrono::steady_clock::now() + fault_duration_;
    }
    timer_cv_.notify_one();
}

void VoltagePlausibilityMonitor::cancel_fault_timer() {
    {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (!timer_armed_) {
            return;
        }
        timer_armed_ = false;
    }
    timer_cv_.notify_one();
}

void VoltagePlausibilityMonitor::timer_thread_func() {
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
        const double std_deviation = timer_std_deviation_snapshot_;
        timer_armed_ = false;

        // Release the lock while invoking the callback path
        lock.unlock();
        trigger_fault(fmt::format("Voltage standard deviation {:.2f} V exceeded threshold {:.2f} V for at least {} ms.",
                                  std_deviation, std_deviation_threshold_V_, fault_duration_.count()));
        lock.lock();
    }
}

} // namespace module
