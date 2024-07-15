// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <unordered_map>

#include <everest/timer.hpp>

#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

#include <ocpp/v201/device_model_storage.hpp>

namespace ocpp::v201 {

class DeviceModel;

struct TriggeredMonitorData {
    VariableMonitoringMeta monitor_meta;
    Component component;
    Variable variable;

    std::string value_previous;
    std::string value_current;

    // \brief Write-only values will not have the value reported
    bool is_writeonly;

    /// \brief If the trigger was sent to the CSMS. We'll keep a copy since we'll also detect
    /// when the monitor returns back to normal
    bool csms_sent;

    /// \brief The trigger has been cleared, that is it returned to normal after a problem
    /// was detected. Can be removed from the map when it was cleared
    bool cleared;
};

struct PeriodicMonitorData {
    VariableMonitoringMeta monitor_meta;
    Component component;
    Variable variable;

    /// \brief Last time we've triggered a periodic delta monitor
    std::chrono::time_point<std::chrono::steady_clock> last_trigger_steady;

    /// \brief Next time when we require to trigger a clock aligned value
    std::chrono::time_point<std::chrono::system_clock> next_trigger_clock_aligned;
};

typedef std::function<void(const std::vector<EventData>&)> notify_events;
typedef std::function<bool()> is_offline;

class MonitoringUpdater {

public:
    MonitoringUpdater() = delete;

    /// \brief Constructs a new variable monitor updater
    /// \param device_model Currently used variable device model
    /// \param notify_csms_events Function that can be invoked with a number of alert events
    /// \param is_chargepoint_offline Function that can be invoked in order to retrieve the
    /// status of the charging station connection to the CSMS
    MonitoringUpdater(std::shared_ptr<DeviceModel> device_model, notify_events notify_csms_events,
                      is_offline is_chargepoint_offline);
    ~MonitoringUpdater();

public:
    /// \brief Starts monitoring the variables, kicking the timer
    void start_monitoring();
    /// \brief Stops monitoring the variables, canceling the timer
    void stop_monitoring();

    /// \brief Processes the variable triggered monitors. Will be called
    /// after relevant variable modification operations or will be called
    /// periodically in case that processing can not be done at the current
    /// moment, for example in the case of an internal variable modification
    void process_triggered_monitors();

private:
    /// \brief Callback that is registered to the 'device_model' that determines if any of
    /// the monitors are triggered for a certain variable when the internal value is used. Will
    /// delay the sending of the monitors to the CSMS until the charging station has
    /// finished any current operation. The reason is that a variable can change during an
    /// operation where the CSMS does NOT expect a message of type 'EventData' therefore
    /// the processing is delayed either until a manual call to 'process_triggered_monitors'
    /// or when the periodic monitoring timer is hit
    void on_variable_changed(const std::unordered_map<int64_t, VariableMonitoringMeta>& monitors,
                             const Component& component, const Variable& variable,
                             const VariableCharacteristics& characteristics, const VariableAttribute& attribute,
                             const std::string& value_old, const std::string& value_current);

    /// \brief Processes the periodic monitors. Since this can be somewhat of a costly
    /// operation (DB query of each triggered monitor's actual value) the processing time
    /// can be configured using the 'VariableMonitoringProcessTime' internal variable. If
    // there are also any pending alert triggered monitors, those will be processed too
    void process_periodic_monitors();

private:
    std::shared_ptr<DeviceModel> device_model;
    Everest::SteadyTimer monitors_timer;

    // Charger to CSMS message unique ID for EventData
    int32_t unique_id;

    notify_events notify_csms_events;
    is_offline is_chargepoint_offline;

    std::unordered_map<int32_t, TriggeredMonitorData> triggered_monitors;
    std::unordered_map<int32_t, PeriodicMonitorData> periodic_monitors;
};

} // namespace ocpp::v201