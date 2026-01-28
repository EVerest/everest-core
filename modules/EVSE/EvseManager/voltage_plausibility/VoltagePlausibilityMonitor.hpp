// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

namespace module {

/// \brief Voltage plausibility monitor used by EvseManager during DC charging.
///
/// The monitor compares voltage measurements from multiple sources (power supply, powermeter,
/// isolation monitor, over voltage monitor) and checks that they report similar values. If the
/// standard deviation between available sources exceeds a configured threshold for a given
/// duration, a fault is raised.
///
/// Thread-safety:
/// - Public APIs are intended to be called from EvseManager threads and from callbacks of the
///   external voltage source interfaces.
/// - Internally, a dedicated background thread waits on a fault timer condition and synchronizes
///   access to timer-related state via an internal mutex and condition variable.
class VoltagePlausibilityMonitor {
public:
    /// \brief Callback type used to report detected faults.
    ///
    /// The callback is invoked from an internal monitoring context when a fault is detected.
    /// It receives a human-readable description of the fault.
    using ErrorCallback = std::function<void(const std::string&)>;

    /// \brief Construct a new VoltagePlausibilityMonitor.
    ///
    /// \param callback  Function that will be called whenever a fault is detected.
    /// \param std_deviation_threshold_V  Maximum allowed standard deviation in volts between
    ///                                   voltage sources before a fault is triggered.
    /// \param fault_duration  Duration for which the standard deviation must exceed the threshold
    ///                        before a fault is raised. A duration of 0 ms means that a fault will
    ///                        be raised immediately when the threshold is exceeded.
    /// \param measurement_max_age  Maximum age of a voltage measurement before it is considered
    ///                               stale and excluded from standard deviation calculation.
    VoltagePlausibilityMonitor(ErrorCallback callback, double std_deviation_threshold_V,
                               std::chrono::milliseconds fault_duration, std::chrono::milliseconds measurement_max_age);

    /// \brief Destructor joins the internal timer thread before destroying the object.
    ~VoltagePlausibilityMonitor();

    /// \brief Start monitoring of incoming voltage samples.
    ///
    /// Clears any latched fault state and cancels a pending fault timer, then enables
    /// evaluation of voltage samples.
    void start_monitor();

    /// \brief Stop monitoring of incoming voltage samples.
    ///
    /// Monitoring is disabled and any pending fault timer is cancelled. Existing latched
    /// faults remain active until \ref reset() is called.
    void stop_monitor();

    /// \brief Feed a new voltage sample from the power supply.
    ///
    /// \param voltage_V  Measured DC voltage in volts.
    void update_power_supply_voltage(double voltage_V);

    /// \brief Feed a new voltage sample from the powermeter.
    ///
    /// \param voltage_V  Measured DC voltage in volts.
    void update_powermeter_voltage(double voltage_V);

    /// \brief Feed a new voltage sample from the isolation monitor.
    ///
    /// \param voltage_V  Measured DC voltage in volts.
    void update_isolation_monitor_voltage(double voltage_V);

    /// \brief Feed a new voltage sample from the over voltage monitor.
    ///
    /// \param voltage_V  Measured DC voltage in volts.
    void update_over_voltage_monitor_voltage(double voltage_V);

    /// \brief Reset the internal fault latch and cancel timers.
    ///
    /// This clears any previously raised fault and stops the internal fault timer. Monitoring
    /// remains disabled until \ref start_monitor() is called again.
    void reset();

private:
    struct VoltageSample {
        double voltage_V;
        std::chrono::steady_clock::time_point timestamp;
    };

    void timer_thread_func();
    void trigger_fault(const std::string& reason);
    void arm_fault_timer(double std_deviation_V);
    void cancel_fault_timer();
    void evaluate_voltages();
    double calculate_standard_deviation(const std::vector<double>& values);

    ErrorCallback error_callback_;
    double std_deviation_threshold_V_;
    std::chrono::milliseconds fault_duration_;
    std::chrono::milliseconds measurement_max_age_;
    bool running_{false};
    bool fault_latched_{false};

    std::mutex data_mutex_;
    VoltageSample power_supply_sample_;
    VoltageSample powermeter_sample_;
    VoltageSample isolation_monitor_sample_;
    VoltageSample over_voltage_monitor_sample_;

    std::mutex timer_mutex_;
    double timer_std_deviation_snapshot_{0.0};
    std::chrono::steady_clock::time_point timer_deadline_{};
    bool timer_armed_{false};
    bool timer_thread_exit_{false};
    std::condition_variable timer_cv_;
    std::thread timer_thread_;
};

} // namespace module
