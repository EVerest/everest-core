// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <limits>
#include <mutex>
#include <string>
#include <thread>

namespace module {

/// \brief Simple software over-voltage watchdog used by EvseManager.
///
/// The monitor observes a DC voltage and compares it against two thresholds:
///
/// - \ref FaultType::Emergency: if the measured voltage is greater than or equal to the
///   configured emergency limit, an emergency fault is raised immediately.
/// - \ref FaultType::Error: if the measured voltage is greater than or equal to the configured
///   error limit continuously for at least the configured duration, an error fault is raised.
///
/// After a fault has been raised, it is latched until \ref reset() is called and monitoring
/// is started again via \ref start_monitor().
///
/// Thread-safety:
/// - Public APIs are intended to be called from EvseManager threads and from callbacks of the
///   external over_voltage_monitor interface.
/// - Internally, a dedicated background thread waits on an error timer condition and synchronizes
///   access to timer-related state via an internal mutex and condition variable.
class OverVoltageMonitor {
public:
    /// \brief Type of over-voltage fault.
    ///
    /// - Error: voltage above the error limit for at least the configured duration.
    /// - Emergency: voltage above the emergency limit, triggers immediately.
    enum class FaultType {
        Error,
        Emergency
    };

    /// \brief Callback type used to report detected faults.
    ///
    /// The callback is invoked from an internal monitoring context when a fault is detected.
    /// It receives the fault type and a human-readable description.
    using ErrorCallback = std::function<void(FaultType, const std::string&)>;

    /// \brief Construct a new OverVoltageMonitor.
    ///
    /// \param callback  Function that will be called whenever a fault is detected.
    /// \param duration  Duration for which the voltage must stay above the error limit before
    ///                  an \ref FaultType::Error is raised. A duration of 0 ms means that an
    ///                  error fault will be raised immediately when the error limit is exceeded.
    OverVoltageMonitor(ErrorCallback callback, std::chrono::milliseconds duration);

    /// \brief Destructor joins the internal timer thread before destroying the object.
    ~OverVoltageMonitor();

    /// \brief Configure the error and emergency voltage limits.
    ///
    /// This must be called before monitoring can become active. Calling this function marks
    /// the limits as valid and enables evaluation in \ref update_voltage().
    ///
    /// \param emergency_limit  Emergency limit in volts; exceeding this immediately triggers an
    ///                         \ref FaultType::Emergency.
    /// \param error_limit      Error limit in volts; exceeding this for at least the configured
    ///                         duration triggers an \ref FaultType::Error.
    void set_limits(double emergency_limit, double error_limit);

    /// \brief Start monitoring of incoming voltage samples.
    ///
    /// Clears any latched fault state and cancels a pending error timer, then enables
    /// evaluation in \ref update_voltage().
    void start_monitor();

    /// \brief Stop monitoring of incoming voltage samples.
    ///
    /// Monitoring is disabled and any pending error timer is cancelled. Existing latched
    /// faults remain active until \ref reset() is called.
    void stop_monitor();

    /// \brief Feed a new voltage sample to the monitor.
    ///
    /// If monitoring is active and limits have been configured, this function evaluates the
    /// sample against the configured error and emergency limits and may:
    ///
    /// - Trigger an immediate emergency fault.
    /// - Arm or update an error timer.
    /// - Cancel an active error timer if the voltage falls back below the error limit.
    ///
    /// \param voltage_v  Measured DC voltage in volts.
    void update_voltage(double voltage_v);

    /// \brief Reset the internal fault latch and cancel timers.
    ///
    /// This clears any previously raised fault and stops the internal error timer. Monitoring
    /// remains disabled until \ref start_monitor() is called again.
    void reset();

private:
    void timer_thread_func();

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

    std::mutex timer_mutex_;
    double timer_voltage_snapshot_{0.0};
    std::chrono::steady_clock::time_point timer_deadline_{};
    bool timer_armed_{false};
    bool timer_thread_exit_{false};
    std::condition_variable timer_cv_;
    std::thread timer_thread_;
};

} // namespace module
