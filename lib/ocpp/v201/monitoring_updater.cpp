// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/monitoring_updater.hpp>

#include <chrono>

#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/utils.hpp>

namespace ocpp::v201 {

template <DataEnum T>
bool triggers_monitor(const VariableMonitoringMeta& monitor_meta, const std::string& value_old,
                      const std::string& value_new) {
    if constexpr (T == DataEnum::boolean) {
        return (value_old != value_new);
    } else {
        auto raw_val_current = to_specific_type_auto<T>(value_old);

        if (monitor_meta.monitor.type == MonitorEnum::Delta) {
            // TODO (ioan): the reference value should never be null but see for ref access except
            auto raw_val_reference = to_specific_type_auto<T>(monitor_meta.reference_value.value());
            auto raw_val_new = to_specific_type_auto<T>(value_new);

            auto delta = std::abs(raw_val_reference - raw_val_new);

            return (delta > monitor_meta.monitor.value);
        } else if (monitor_meta.monitor.type == MonitorEnum::LowerThreshold) {
            return (raw_val_current < monitor_meta.monitor.value);
        } else if (monitor_meta.monitor.type == MonitorEnum::UpperThreshold) {
            return (raw_val_current > monitor_meta.monitor.value);
        } else {
            EVLOG_error << "Requested unsupported trigger monitor of type: "
                        << conversions::monitor_enum_to_string(monitor_meta.monitor.type);
            return false;
        }
    }

    return false;
}

bool is_monitor_active(MonitoringBaseEnum active_monitoring_base, const VariableMonitoringMeta& monitor_meta) {
    // Skip monitors that are not active
    if (active_monitoring_base != MonitoringBaseEnum::All) {
        // If we have the factory default option, skip all
        // CustomMonitors (installed by the CSMS)
        if (active_monitoring_base == MonitoringBaseEnum::FactoryDefault &&
            monitor_meta.type == VariableMonitorType::CustomMonitor) {
            return false;
        }

        // If we have the hardwired option, skip all non-hardwired
        // monitors (that is CustomMonitor and PreconfiguredMonitor)
        if (active_monitoring_base == MonitoringBaseEnum::HardWiredOnly &&
            monitor_meta.type != VariableMonitorType::HardWiredMonitor) {
            return false;
        }
    }

    return true;
}

std::chrono::time_point<std::chrono::system_clock> get_next_clock_aligned_point(float monitor_interval) {
    auto monitor_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<float>(monitor_interval));

    auto sys_time_now = std::chrono::system_clock::now();
    auto hours_now = std::chrono::floor<std::chrono::hours>(sys_time_now);
    auto seconds_now = std::chrono::duration_cast<std::chrono::seconds>(sys_time_now - hours_now);

    // Round next seconds, for ex at an interval of 900 while we are at second 2700 will yield
    // the result is 3600, and that is a roll-over, we will call the next monitor at the precise hour
    auto next_seconds =
        (std::ceil((double)seconds_now.count() / (double)monitor_seconds.count()) * monitor_seconds).count();

    std::chrono::time_point<std::chrono::system_clock> aligned_timepoint;

    if (next_seconds >= static_cast<decltype(next_seconds)>(3600)) {
        // If we rolled over, move to the next hour
        aligned_timepoint = (hours_now + std::chrono::hours(1));
    } else {
        aligned_timepoint = (hours_now + std::chrono::duration_cast<std::chrono::seconds>(
                                             std::chrono::duration<decltype(next_seconds)>(next_seconds)));
    }

    auto dbg_time_now = std::chrono::system_clock::to_time_t(sys_time_now);
    auto dbg_time_aligned = std::chrono::system_clock::to_time_t(aligned_timepoint);
    EVLOG_debug << "Aligned time: " << std::ctime(&dbg_time_now) << " with interval: " << monitor_seconds.count()
                << " to next timepoint: " << std::ctime(&dbg_time_aligned);

    return aligned_timepoint;
}

EventData create_notify_event(int32_t unique_id, const std::string& reported_value, const Component& component,
                              const Variable& variable, const VariableMonitoringMeta& monitor_meta) {
    EventData notify_event;

    notify_event.component = component;
    notify_event.variable = variable;
    notify_event.variableMonitoringId = monitor_meta.monitor.id;

    notify_event.eventId = unique_id;
    notify_event.timestamp = ocpp::DateTime();
    notify_event.actualValue = reported_value;

    if (monitor_meta.monitor.type == MonitorEnum::Periodic ||
        monitor_meta.monitor.type == MonitorEnum::PeriodicClockAligned) {
        notify_event.trigger = EventTriggerEnum::Periodic;
    } else if (monitor_meta.monitor.type == MonitorEnum::Delta) {
        notify_event.trigger = EventTriggerEnum::Delta;
    } else if (monitor_meta.monitor.type == MonitorEnum::UpperThreshold ||
               monitor_meta.monitor.type == MonitorEnum::LowerThreshold) {
        notify_event.trigger = EventTriggerEnum::Alerting;
    } else {
        EVLOG_error << "Invalid monitor type of: " << conversions::monitor_enum_to_string(monitor_meta.monitor.type);
    }

    if (monitor_meta.type == VariableMonitorType::HardWiredMonitor) {
        notify_event.eventNotificationType = EventNotificationEnum::HardWiredMonitor;
    } else if (monitor_meta.type == VariableMonitorType::PreconfiguredMonitor) {
        notify_event.eventNotificationType = EventNotificationEnum::PreconfiguredMonitor;
    } else if (monitor_meta.type == VariableMonitorType::CustomMonitor) {
        notify_event.eventNotificationType = EventNotificationEnum::CustomMonitor;
    } else {
        EVLOG_error << "Invalid monitor meta type of: " << static_cast<int>(monitor_meta.type);
    }

    return notify_event;
}

MonitoringUpdater::MonitoringUpdater(std::shared_ptr<DeviceModel> device_model, notify_events notify_csms_events,
                                     is_offline is_chargepoint_offline) :
    device_model(std::move(device_model)),
    notify_csms_events(std::move(notify_csms_events)),
    is_chargepoint_offline(std::move(is_chargepoint_offline)),
    monitors_timer([this]() { this->process_periodic_monitors(); }),
    unique_id(0) {
}

MonitoringUpdater::~MonitoringUpdater() {
    stop_monitoring();
}

void MonitoringUpdater::start_monitoring() {
    // Bind function to this instance
    auto fn = std::bind(&MonitoringUpdater::on_variable_changed, this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,
                        std::placeholders::_7);
    device_model->register_variable_listener(std::move(fn));

    // No point in starting the monitor if this variable does not exist. It will never start to exist later on.
    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::MonitoringCtrlrEnabled)
            .has_value()) {
        int process_interval_seconds =
            this->device_model->get_optional_value<int>(ControllerComponentVariables::MonitorsProcessingInterval)
                .value_or(1);

        monitors_timer.interval(std::chrono::seconds(process_interval_seconds));
    }
}

void MonitoringUpdater::stop_monitoring() {
    monitors_timer.stop();
}

void MonitoringUpdater::on_variable_changed(const std::unordered_map<int64_t, VariableMonitoringMeta>& monitors,
                                            const Component& component, const Variable& variable,
                                            const VariableCharacteristics& characteristics,
                                            const VariableAttribute& attribute, const std::string& value_previous,
                                            const std::string& value_current) {
    EVLOG_debug << "Variable: " << variable.name.get() << " changed value from: [" << value_previous << "] to: ["
                << value_current << "]";

    // Ignore non-actual values
    if (attribute.type.has_value() && attribute.type.value() != AttributeEnum::Actual) {
        return;
    }

    // Iterate monitors and search for a triggered monitor
    for (const auto& [monitor_id, monitor_meta] : monitors) {
        // Don't care about periodic
        switch (monitor_meta.monitor.type) {
        case MonitorEnum::Periodic:
        case MonitorEnum::PeriodicClockAligned:
            continue;
        }

        bool monitor_triggered = false;
        bool monitor_trivial = false;

        // N07.FR.19 - Based on this it seems that OptionList, SequenceList, MemberList will
        // cause a trigger if the value is changed regardless of the content (or monitor delta)
        if ((characteristics.dataType == DataEnum::boolean) || (characteristics.dataType == DataEnum::string) ||
            (characteristics.dataType == DataEnum::dateTime) || (characteristics.dataType == DataEnum::OptionList) ||
            (characteristics.dataType == DataEnum::MemberList) ||
            (characteristics.dataType == DataEnum::SequenceList)) {
            monitor_triggered = triggers_monitor<DataEnum::boolean>(monitor_meta, value_previous, value_current);
            monitor_trivial = true;
        } else if (characteristics.dataType == DataEnum::decimal) {
            monitor_triggered = triggers_monitor<DataEnum::decimal>(monitor_meta, value_previous, value_current);
        } else if (characteristics.dataType == DataEnum::integer) {
            monitor_triggered = triggers_monitor<DataEnum::integer>(monitor_meta, value_previous, value_current);
        } else {
            EVLOG_error << "Requested unsupported 'DataEnum' type: "
                        << conversions::data_enum_to_string(characteristics.dataType);
            continue;
        }

        EVLOG_debug << "Monitor: [" << monitor_meta.monitor << "] triggered: " << monitor_triggered;

        auto it = triggered_monitors.find(monitor_id);

        if (monitor_triggered) {
            if (monitor_meta.monitor.type == MonitorEnum::Delta && monitor_trivial) {
                // 3.55. MonitorEnumType
                // As per the spec, in case of a delta monitor that always triggered (bool/dateTime etc...)
                // we must update the reference value to the new value, so that we don't always trigger
                // this multiple times when it changes
                try {
                    if (!this->device_model->update_monitor_reference(monitor_id, value_current)) {
                        EVLOG_warning << "Could not update delta monitor: " << monitor_id << " reference!";
                    }
                } catch (const DeviceModelStorageError& e) {
                    EVLOG_error << "Could not update delta monitor reference with exception: " << e.what();
                }
            }

            if (it == std::end(triggered_monitors)) {
                TriggeredMonitorData triggered;

                triggered.component = component;
                triggered.variable = variable;
                triggered.monitor_meta = monitor_meta;
                triggered.csms_sent = false;
                triggered.cleared = false;
                triggered.is_writeonly =
                    (attribute.mutability.value_or(MutabilityEnum::ReadWrite) == MutabilityEnum::WriteOnly);

                auto res = triggered_monitors.insert(std::pair{monitor_meta.monitor.id, std::move(triggered)});
                if (!res.second) {
                    EVLOG_warning << "Could not insert monitor to triggered monitor map!";
                    continue;
                }
                it = res.first;

                EVLOG_debug << "Variable: " << variable.name.get() << " triggered monitor: " << monitor_id;
            }

            TriggeredMonitorData& triggered_data = it->second;

            // If we are in a 'not dangerous' a.k.a 'cleared' state
            if (triggered_data.cleared) {
                // Make it not cleared, that is in a 'dangerous' state
                triggered_data.cleared = false;
                // Also reset the CSMS sent, since the new cleared state was not sent
                triggered_data.csms_sent = false;

                EVLOG_debug << "Variable: " << variable.name.get() << " triggered monitor: " << monitor_id;
            }

            // Update relevant values only
            triggered_data.value_previous = value_previous;
            triggered_data.value_current = value_current;
        } else {
            // If the monitor is not triggered and we already have the data
            // in our triggered list it means that we have returned to normal
            if (it != std::end(triggered_monitors)) {
                if (!it->second.cleared) {
                    // Mark as cleared (a.k.a normal)
                    it->second.cleared = true;

                    // If it was previously sent to the CSMS, mark it as not sent
                    // so that we can allow the CSMS to know that we had a return to normal
                    it->second.csms_sent = false;

                    EVLOG_debug << "Variable: " << variable.name.get() << " cleared monitor: " << monitor_id;
                }
            }
        }
    }
}

void MonitoringUpdater::process_periodic_monitors() {
    bool monitoring_enabled =
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::MonitoringCtrlrEnabled)
            .value_or(false);

    if (!monitoring_enabled) {
        return;
    }

    if (!triggered_monitors.empty()) {
        process_triggered_monitors();
    }

    // We don't persist the messages here, base on the  'OfflineQueuingSeverity'
    // since the periodic monitors do not imply an alert
    if (is_chargepoint_offline()) {
        return;
    }

    int active_monitoring_level =
        this->device_model->get_optional_value<int>(ControllerComponentVariables::ActiveMonitoringLevel)
            .value_or(MontoringLevelSeverity::MAX);

    std::string active_monitoring_base_string =
        this->device_model->get_optional_value<std::string>(ControllerComponentVariables::ActiveMonitoringBase)
            .value_or(conversions::monitoring_base_enum_to_string(MonitoringBaseEnum::All));
    MonitoringBaseEnum active_monitoring_base =
        conversions::string_to_monitoring_base_enum(active_monitoring_base_string);

    EVLOG_debug << "Processing periodic monitors";

    std::vector<EventData> monitor_events;
    auto monitors = this->device_model->get_periodic_monitors();

    for (auto& component_variable_monitors : monitors) {
        for (auto& periodic_monitor_meta : component_variable_monitors.monitors) {

            // Skip non-active monitors
            if (!is_monitor_active(active_monitoring_base, periodic_monitor_meta)) {
                continue;
            }

            // Skip monitors that have a severity > than our active monitoring level
            if (periodic_monitor_meta.monitor.severity > active_monitoring_level) {
                continue;
            }

            // Monitor secodns interval
            auto monitor_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::duration<float>(periodic_monitor_meta.monitor.value));

            // See if we already have the local monitor
            auto it = this->periodic_monitors.find(periodic_monitor_meta.monitor.id);

            if (it == std::end(this->periodic_monitors)) {
                PeriodicMonitorData periodic_monitor_data;

                periodic_monitor_data.component = component_variable_monitors.component;
                periodic_monitor_data.variable = component_variable_monitors.variable;
                periodic_monitor_data.monitor_meta = periodic_monitor_meta;

                if (periodic_monitor_meta.monitor.type == MonitorEnum::Periodic) {
                    // Set the trigger to the current time
                    periodic_monitor_data.last_trigger_steady = std::chrono::steady_clock::now();
                } else if (periodic_monitor_meta.monitor.type == MonitorEnum::PeriodicClockAligned) {
                    // Snap to the closest monitor multiple
                    periodic_monitor_data.next_trigger_clock_aligned =
                        get_next_clock_aligned_point(periodic_monitor_data.monitor_meta.monitor.value);
                    EVLOG_debug << "First aligned timepoint for monitor ID: " << periodic_monitor_meta.monitor.id;
                }

                auto res = this->periodic_monitors.insert(
                    std::pair{periodic_monitor_meta.monitor.id, std::move(periodic_monitor_data)});

                if (!res.second) {
                    EVLOG_warning << "Could not insert monitor to periodic monitor map!";
                    continue;
                }

                it = res.first;
            }

            PeriodicMonitorData& periodic_monitor = it->second;
            bool matches_time = false;

            if (periodic_monitor.monitor_meta.monitor.type == MonitorEnum::Periodic) {
                auto current_time = std::chrono::steady_clock::now();
                auto delta = current_time - periodic_monitor.last_trigger_steady;

                if (delta > monitor_seconds) {
                    // Update last time
                    periodic_monitor.last_trigger_steady = current_time;
                    matches_time = true;
                }
            } else if (periodic_monitor.monitor_meta.monitor.type == MonitorEnum::PeriodicClockAligned) {
                // 3.55
                // PeriodicClockAligned Triggers an event notice every monitorValue
                // seconds interval, starting from the nearest clock-aligned interval
                // after this monitor was set. For example, a monitorValue of 900 will
                // trigger event notices at 0, 15, 30 and 45 minutes after the hour, every hour.
                auto current_time = std::chrono::system_clock::now();

                if (current_time > periodic_monitor.next_trigger_clock_aligned) {
                    auto distance = std::chrono::duration_cast<std::chrono::seconds>(
                                        current_time - periodic_monitor.next_trigger_clock_aligned)
                                        .count();

                    // TODO (ioan): Handle with: N08.FR.03, events should be queued and
                    // send when the charger is back online
                    if (distance > static_cast<decltype(distance)>(60)) {
                        EVLOG_warning << "Missed scheduled monitor time by: " << distance;
                    }
                    matches_time = true;
                    EVLOG_debug << "Reporting periodic monitor with id: " << periodic_monitor_meta.monitor.id;

                    periodic_monitor.next_trigger_clock_aligned =
                        get_next_clock_aligned_point(periodic_monitor.monitor_meta.monitor.value);
                }
            } else {
                EVLOG_error << "Invalid monitor type from: 'get_periodic_monitors': "
                            << conversions::monitor_enum_to_string(periodic_monitor.monitor_meta.monitor.type);
                continue;
            }

            if (matches_time) {
                RequiredComponentVariable comp_var;
                comp_var.component = component_variable_monitors.component;
                comp_var.variable = component_variable_monitors.variable;

                std::string current_value = this->device_model->get_value<std::string>(comp_var);

                EventData notify_event = std::move(
                    create_notify_event(this->unique_id++, current_value, component_variable_monitors.component,
                                        component_variable_monitors.variable, periodic_monitor.monitor_meta));

                monitor_events.push_back(std::move(notify_event));
            }
        }
    }

    if (!monitor_events.empty()) {
        notify_csms_events(monitor_events);
    }
}

void MonitoringUpdater::process_triggered_monitors() {
    bool monitoring_enabled =
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::MonitoringCtrlrEnabled)
            .value_or(false);

    if (!monitoring_enabled) {
        return;
    }

    EVLOG_debug << "Processing alert monitors";

    // Persist OfflineMonitoringEventQueuingSeverity even when offline if we have a problem
    bool is_offline = is_chargepoint_offline();
    // By default (if the comp is missing we are reporting up to 'Warning')
    int offline_severity =
        this->device_model->get_optional_value<int>(ControllerComponentVariables::OfflineQueuingSeverity)
            .value_or(MontoringLevelSeverity::Warning);

    int active_monitoring_level =
        this->device_model->get_optional_value<int>(ControllerComponentVariables::ActiveMonitoringLevel)
            .value_or(MontoringLevelSeverity::MAX);

    std::string active_monitoring_base_string =
        this->device_model->get_optional_value<std::string>(ControllerComponentVariables::ActiveMonitoringBase)
            .value_or(conversions::monitoring_base_enum_to_string(MonitoringBaseEnum::All));
    MonitoringBaseEnum active_monitoring_base =
        conversions::string_to_monitoring_base_enum(active_monitoring_base_string);

    for (auto it = triggered_monitors.begin(); it != triggered_monitors.end();) {
        bool should_clear = false;

        // If the monitor is active process it
        if (is_monitor_active(active_monitoring_base, it->second.monitor_meta)) {
            if (is_offline) {
                // If we are offline, just discard triggers that have a severity > than 'offline_severity'
                if (it->second.monitor_meta.monitor.severity > offline_severity) {
                    should_clear = true;
                }
            } else {
                // If we are online, discard the triggers that have a severity > than 'active_monitoring_level'
                if (it->second.monitor_meta.monitor.severity > active_monitoring_level) {
                    should_clear = true;
                }
            }
        } else {
            // If the monitor is not active, simply discard
            should_clear = true;
        }

        if (should_clear) {
            EVLOG_debug << "Erased triggered monitor: [" << it->second.component << ":" << it->second.variable
                        << "] since we're offline and the severity is < 'OfflineQueuingSeverity'";
            it = triggered_monitors.erase(it);
        } else {
            ++it;
        }
    }

    if (!is_offline) {
        std::vector<EventData> monitor_events;
        for (auto& [id, trigger] : triggered_monitors) {
            // Only send a trigger if we did not already sent it, as we
            // will mark it as not sent again, after the value changes
            if (trigger.csms_sent == false) {
                std::string reported_value{};

                // If the variable is marked as read-only then the value will NOT be reported
                if (trigger.is_writeonly == false) {
                    reported_value = trigger.value_current;
                }

                EventData notify_event = std::move(create_notify_event(unique_id++, reported_value, trigger.component,
                                                                       trigger.variable, trigger.monitor_meta));
                // Mark if the event is cleared (returned to normal)
                if (trigger.cleared) {
                    notify_event.cleared = trigger.cleared;
                }

                // Mark the triggers as CSMS sent
                trigger.csms_sent = true;

                monitor_events.push_back(std::move(notify_event));
            }
        }

        if (!monitor_events.empty()) {
            notify_csms_events(monitor_events);
        }
    }

    for (auto it = triggered_monitors.begin(); it != triggered_monitors.end();) {
        bool should_clear = false;

        if (it->second.csms_sent == false && it->second.cleared == true) {
            // Based on spec comments, if a variable returns to normal and we're offline and
            // it was not sent to the CSMS it can safely be discarded:
            // "Only those monitoring events that were triggered & cleared during the offline period will remain
            // invisible to CSMS."
            should_clear = true;
        } else if (it->second.csms_sent && it->second.cleared == true) {
            // Also discard monitors that were sent to the CSMS and are cleared,
            // that is, in their proper state
            should_clear = true;
        }

        if (should_clear) {
            EVLOG_debug << "Erased triggered monitor: [" << it->second.component << ":" << it->second.variable
                        << "] since it was either cleared or it was sent to the CSMS and cleared";
            it = triggered_monitors.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace ocpp::v201