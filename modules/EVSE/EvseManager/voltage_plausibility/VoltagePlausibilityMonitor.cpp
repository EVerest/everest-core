// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "voltage_plausibility/VoltagePlausibilityMonitor.hpp"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <everest/logging.hpp>
#include <fmt/core.h>
#include <thread>

namespace module {

VoltagePlausibilityMonitor::VoltagePlausibilityMonitor(ErrorCallback callback, double max_spread_threshold_V,
                                                       std::chrono::milliseconds fault_duration) :
    error_callback_(std::move(callback)),
    max_spread_threshold_V_(max_spread_threshold_V),
    fault_duration_(fault_duration) {
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
    EVLOG_info << "VoltagePlausibilityMonitor, start monitoring";
    fault_latched_.store(false);
    cancel_fault_timer();
    running_.store(true);
}

void VoltagePlausibilityMonitor::stop_monitor() {
    EVLOG_info << "VoltagePlausibilityMonitor, stop monitoring";
    running_.store(false);
    cancel_fault_timer();
}

void VoltagePlausibilityMonitor::reset() {
    fault_latched_.store(false);
    {
        // Reset all voltage samples to zero time - protected by data_mutex_
        std::lock_guard<std::mutex> lock(data_mutex_);
        power_supply_sample_.timestamp = std::chrono::steady_clock::time_point{};
        powermeter_sample_.timestamp = std::chrono::steady_clock::time_point{};
        isolation_monitor_sample_.timestamp = std::chrono::steady_clock::time_point{};
        over_voltage_monitor_sample_.timestamp = std::chrono::steady_clock::time_point{};
    }
    cancel_fault_timer();
}

void VoltagePlausibilityMonitor::update_power_supply_voltage(double voltage_V) {
    std::vector<double> valid_voltages;
    if (running_.load() && !fault_latched_.load()) {
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            power_supply_sample_.voltage_V = voltage_V;
            power_supply_sample_.timestamp = std::chrono::steady_clock::now();
            valid_voltages = get_valid_voltages();
        }
        evaluate_voltages(valid_voltages);
    }
}

void VoltagePlausibilityMonitor::update_powermeter_voltage(double voltage_V) {
    std::vector<double> valid_voltages;
    if (running_.load() && !fault_latched_.load()) {
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            powermeter_sample_.voltage_V = voltage_V;
            powermeter_sample_.timestamp = std::chrono::steady_clock::now();
            valid_voltages = get_valid_voltages();
        }
        evaluate_voltages(valid_voltages);
    }
}

void VoltagePlausibilityMonitor::update_isolation_monitor_voltage(double voltage_V) {
    std::vector<double> valid_voltages;
    if (running_.load() && !fault_latched_.load()) {
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            isolation_monitor_sample_.voltage_V = voltage_V;
            isolation_monitor_sample_.timestamp = std::chrono::steady_clock::now();
            valid_voltages = get_valid_voltages();
        }
        evaluate_voltages(valid_voltages);
    }
}

void VoltagePlausibilityMonitor::update_over_voltage_monitor_voltage(double voltage_V) {
    std::vector<double> valid_voltages;
    if (running_.load() && !fault_latched_.load()) {
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            over_voltage_monitor_sample_.voltage_V = voltage_V;
            over_voltage_monitor_sample_.timestamp = std::chrono::steady_clock::now();
            valid_voltages = get_valid_voltages();
        }
        evaluate_voltages(valid_voltages);
    }
}

std::vector<double> VoltagePlausibilityMonitor::get_valid_voltages() const {
    std::vector<double> valid_voltages;
    const auto zero_time = std::chrono::steady_clock::time_point{};

    // Collect voltage samples that were updated since the last reset/start.
    // A timestamp of zero (default-initialized) means we've never received a value (or it was cleared on reset()).
    if (power_supply_sample_.timestamp != zero_time) {
        valid_voltages.push_back(power_supply_sample_.voltage_V);
    }

    if (powermeter_sample_.timestamp != zero_time) {
        valid_voltages.push_back(powermeter_sample_.voltage_V);
    }

    if (isolation_monitor_sample_.timestamp != zero_time) {
        valid_voltages.push_back(isolation_monitor_sample_.voltage_V);
    }

    if (over_voltage_monitor_sample_.timestamp != zero_time) {
        valid_voltages.push_back(over_voltage_monitor_sample_.voltage_V);
    }
    return valid_voltages;
}

void VoltagePlausibilityMonitor::evaluate_voltages(const std::vector<double>& valid_voltages) {
    // Need at least 2 valid sources to compute spread
    if (valid_voltages.size() < 2) {
        const bool cancelled = cancel_fault_timer();
        if (cancelled) {
            EVLOG_info << "VoltagePlausibilityMonitor, using less than 2 valid voltage sources -> cancelling timer";
        }
        return;
    }

    // Order values and compute spread (max-min)
    const double spread_V = calculate_spread(valid_voltages);
    // Check against threshold
    if (spread_V > max_spread_threshold_V_) {
        const bool armed = arm_fault_timer(spread_V);
        if (armed) {
            EVLOG_info << "VoltagePlausibilityMonitor, spread: " << spread_V
                       << " V > threshold: " << max_spread_threshold_V_ << " V -> arming timer";
        }
    } else {
        const bool cancelled = cancel_fault_timer();
        if (cancelled) {
            EVLOG_info << "VoltagePlausibilityMonitor, spread: " << spread_V
                       << " V <= threshold: " << max_spread_threshold_V_ << " V -> cancelling timer";
        }
    }
}

double VoltagePlausibilityMonitor::calculate_spread(std::vector<double> valid_voltages) {
    if (valid_voltages.size() < 2) {
        return 0.0;
    }
    std::sort(valid_voltages.begin(), valid_voltages.end());
    return valid_voltages.back() - valid_voltages.front();
}

void VoltagePlausibilityMonitor::trigger_fault(const std::string& reason) {
    fault_latched_.store(true);
    running_.store(false);
    cancel_fault_timer();
    if (error_callback_) {
        error_callback_(reason);
    }
}

bool VoltagePlausibilityMonitor::arm_fault_timer(double spread_V) {
    if (fault_duration_.count() == 0) {
        trigger_fault(fmt::format(
            "Voltage spread {:.2f} V exceeded threshold {:.2f} V and fault duration is 0 ms -> fault immediately",
            spread_V, max_spread_threshold_V_));
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (timer_armed_) {
            return false;
        }
        timer_armed_ = true;
        timer_deadline_ = std::chrono::steady_clock::now() + fault_duration_;
    }
    timer_cv_.notify_one();
    return true;
}

bool VoltagePlausibilityMonitor::cancel_fault_timer() {
    bool cancelled_now = false;
    {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (!timer_armed_) {
            return false;
        }
        timer_armed_ = false;
        cancelled_now = true;
    }
    timer_cv_.notify_one();
    return cancelled_now;
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

        // Timer expired with this deadline and is still armed.
        // Re-check the fault condition using the *current* set of valid voltages. This avoids triggering a fault if
        // updates stopped and we no longer have enough sources (or monitoring stopped) while the timer was armed.
        timer_armed_ = false;

        // Release the lock while invoking the callback path
        lock.unlock();
        bool should_trigger = false;
        double current_spread_V = 0.0;
        if (running_.load() && !fault_latched_.load()) {
            std::vector<double> valid_voltages;
            {
                std::lock_guard<std::mutex> data_lock(data_mutex_);
                valid_voltages = get_valid_voltages();
            }
            if (valid_voltages.size() >= 2) {
                current_spread_V = calculate_spread(std::move(valid_voltages));
                should_trigger = current_spread_V > max_spread_threshold_V_;
            }
        }

        if (should_trigger) {
            trigger_fault(fmt::format("Voltage spread {:.2f} V exceeded threshold {:.2f} V for at least {} ms.",
                                      current_spread_V, max_spread_threshold_V_, fault_duration_.count()));
        } else {
            EVLOG_info << "VoltagePlausibilityMonitor, timer expired but fault condition no longer true -> no fault";
        }
        lock.lock();
    }
}

} // namespace module
