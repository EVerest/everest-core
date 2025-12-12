// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <chrono>
#include <functional>
#include <limits>
#include <mutex>
#include <string>
#include <utility>

namespace module {

class OverVoltageMonitor {
public:
    enum class FaultType {
        Error,
        Emergency
    };
    using ErrorCallback = std::function<void(FaultType, const std::string&)>;

    OverVoltageMonitor(ErrorCallback callback, std::chrono::milliseconds duration);

    void set_limits(double emergency_limit, double error_limit);
    void start_monitor();
    void stop_monitor();
    void update_voltage(double voltage_v);
    void reset();

private:
    void trigger_fault(FaultType type, const std::string& reason);
    void arm_error_timer(double voltage_v);
    void cancel_error_timer();

    ErrorCallback error_callback_;
    std::chrono::milliseconds duration_;
    bool running_{false};
    bool limits_valid_{false};
    bool fault_latched_{false};
    double emergency_limit_{std::numeric_limits<double>::infinity()};
    double error_limit_{std::numeric_limits<double>::infinity()};
    double last_voltage_{0.0};

    std::mutex timer_mutex_;
    bool error_timer_active_{false};
    uint64_t timer_sequence_{0};
    uint64_t current_timer_sequence_{0};
    double timer_voltage_snapshot_{0.0};
};

} // namespace module
