// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef LEM_DCBM_TEMPERATURE_MONITOR_HPP
#define LEM_DCBM_TEMPERATURE_MONITOR_HPP

#include <algorithm>
#include <chrono>
#include <optional>

namespace module::main {

/// Monitors temperature readings against warning and error thresholds
/// with hysteresis and a minimum exceedance duration before raising events.
class TemperatureMonitor {
public:
    struct Config {
        double warning_level_C;
        double error_level_C;
        double hysteresis_K;
        std::chrono::milliseconds min_time_as_valid;
    };

    struct Events {
        bool warning_raised{false};
        bool warning_cleared{false};
        bool error_raised{false};
        bool error_cleared{false};
    };

    explicit TemperatureMonitor(const Config& config) : config_(config) {
    }

    /// Feed new temperature readings and get back any state-change events.
    /// The evaluation uses max(temperature_H, temperature_L).
    Events update(double temperature_H_C, double temperature_L_C) {
        last_max_temperature_ = std::max(temperature_H_C, temperature_L_C);
        const auto now = std::chrono::steady_clock::now();

        Events events;
        evaluate_level(warning_active_, warning_exceeded_since_, config_.warning_level_C, now, events.warning_raised,
                       events.warning_cleared);
        evaluate_level(error_active_, error_exceeded_since_, config_.error_level_C, now, events.error_raised,
                       events.error_cleared);
        return events;
    }

    /// Returns the current max temperature from the last update (for logging).
    [[nodiscard]] double last_max_temperature() const {
        return last_max_temperature_;
    }

private:
    Config config_;

    bool warning_active_{false};
    std::optional<std::chrono::steady_clock::time_point> warning_exceeded_since_;

    bool error_active_{false};
    std::optional<std::chrono::steady_clock::time_point> error_exceeded_since_;

    double last_max_temperature_{0.0};

    void evaluate_level(bool& active, std::optional<std::chrono::steady_clock::time_point>& exceeded_since,
                        double threshold, std::chrono::steady_clock::time_point now, bool& raised_event,
                        bool& cleared_event) {

        if (!active) {
            // Not yet active — check if we should start or continue timing
            if (last_max_temperature_ >= threshold) {
                if (!exceeded_since.has_value()) {
                    // First time exceeding: start the timer
                    exceeded_since = now;
                }
                // Check if minimum exceedance duration has elapsed
                if ((now - exceeded_since.value()) >= config_.min_time_as_valid) {
                    active = true;
                    exceeded_since.reset();
                    raised_event = true;
                }
            } else {
                // Temperature dropped below threshold before timer expired — reset
                exceeded_since.reset();
            }
        } else {
            // Active — check if we should clear (with hysteresis)
            if (last_max_temperature_ < (threshold - config_.hysteresis_K)) {
                active = false;
                exceeded_since.reset();
                cleared_event = true;
            }
        }
    }
};

} // namespace module::main

#endif // LEM_DCBM_TEMPERATURE_MONITOR_HPP
