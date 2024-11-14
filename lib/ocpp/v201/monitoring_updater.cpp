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
        auto raw_val_current = to_specific_type_auto<T>(value_new);

        if (monitor_meta.monitor.type == MonitorEnum::Delta) {
            if (monitor_meta.reference_value.has_value()) {
                auto raw_val_reference = to_specific_type_auto<T>(monitor_meta.reference_value.value());
                auto delta = std::abs(raw_val_reference - raw_val_current);

                return (delta > monitor_meta.monitor.value);
            } else {
                EVLOG_error << "Invalid reference value for monitor: " << monitor_meta.monitor;
                return false;
            }
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
    monitors_timer([this]() { this->process_monitors_internal(true, true); }),
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

    auto fn_monitor =
        std::bind(&MonitoringUpdater::on_monitor_updated, this, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
    device_model->register_monitor_listener(std::move(fn_monitor));

    // No point in starting the monitor if this variable does not exist. It will never start to exist later on.
    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::MonitoringCtrlrEnabled)
            .value_or(false)) {
        int process_interval_seconds =
            this->device_model->get_optional_value<int>(ControllerComponentVariables::MonitorsProcessingInterval)
                .value_or(1);

        EVLOG_info << "Started monitoring timer with interval: " << process_interval_seconds;
        monitors_timer.interval(std::chrono::seconds(process_interval_seconds));
    } else {
        EVLOG_warning << "Attempted to start monitoring without 'MonitoringCtrlrEnabled'";
    }
}

void MonitoringUpdater::stop_monitoring() {
    monitors_timer.stop();
}

void MonitoringUpdater::process_triggered_monitors() {
    this->process_monitors_internal(false, true);
}

void MonitoringUpdater::on_monitor_updated(const VariableMonitoringMeta& updated_monitor, const Component& component,
                                           const Variable& variable, const VariableCharacteristics& characteristics,
                                           const VariableAttribute& attribute, const std::string& current_value) {
    auto it = updater_monitors_meta.find(updated_monitor.monitor.id);

    // Not contained, ignored
    if (it == std::end(updater_monitors_meta)) {
        return;
    }

    auto& meta = it->second;

    // Refresh monitor
    meta.monitor_meta = updated_monitor;

    // N07.FR.11 - based on this we need to re-evaluate the monitor for
    // the Lower/UpperThreshold types
    if (updated_monitor.monitor.type == MonitorEnum::LowerThreshold ||
        updated_monitor.monitor.type == MonitorEnum::UpperThreshold) {
        // Re-evaluate the monitor
        evaluate_monitor(updated_monitor, component, variable, characteristics, attribute, current_value,
                         current_value);
    }
}

void MonitoringUpdater::evaluate_monitor(const VariableMonitoringMeta& monitor_meta, const Component& component,
                                         const Variable& variable, const VariableCharacteristics& characteristics,
                                         const VariableAttribute& attribute, const std::string& value_previous,
                                         const std::string& value_current) {
    // Don't care about periodic
    switch (monitor_meta.monitor.type) {
    case MonitorEnum::Periodic:
    case MonitorEnum::PeriodicClockAligned:
        return;
    }

    bool monitor_triggered = false;
    bool monitor_trivial = false;

    // N07.FR.19 - Based on this it seems that OptionList, SequenceList, MemberList will
    // cause a trigger if the value is changed regardless of the content (or monitor delta)
    if ((characteristics.dataType == DataEnum::boolean) || (characteristics.dataType == DataEnum::string) ||
        (characteristics.dataType == DataEnum::dateTime) || (characteristics.dataType == DataEnum::OptionList) ||
        (characteristics.dataType == DataEnum::MemberList) || (characteristics.dataType == DataEnum::SequenceList)) {
        monitor_triggered = triggers_monitor<DataEnum::boolean>(monitor_meta, value_previous, value_current);
        monitor_trivial = true;
    } else if (characteristics.dataType == DataEnum::decimal) {
        monitor_triggered = triggers_monitor<DataEnum::decimal>(monitor_meta, value_previous, value_current);
    } else if (characteristics.dataType == DataEnum::integer) {
        monitor_triggered = triggers_monitor<DataEnum::integer>(monitor_meta, value_previous, value_current);
    } else {
        EVLOG_error << "Requested unsupported 'DataEnum' type: "
                    << conversions::data_enum_to_string(characteristics.dataType);
        return;
    }

    EVLOG_debug << "Monitor: " << monitor_meta.monitor << " was triggered on var change: [" << monitor_triggered
                << "] with previous value: [" << value_previous << "] and current: [" << value_current << "]";

    auto monitor_id = monitor_meta.monitor.id;
    auto it = updater_monitors_meta.find(monitor_id);

    // Always update the current values is the trigger is found
    if (it != std::end(updater_monitors_meta)) {
        auto& triggered_meta = it->second;

        triggered_meta.value_previous = value_previous;
        triggered_meta.value_current = value_current;
    }

    if (monitor_triggered) {
        if (monitor_meta.monitor.type == MonitorEnum::Delta && monitor_trivial) {
            // 3.55. MonitorEnumType
            // As per the spec, in case of a delta monitor that always triggered (bool/dateTime etc...)
            // we must update the reference value to the new value, so that we don't always trigger
            // this multiple times when it changes

            // N07.FR.18 - "plus or minus monitorValue since the time that this monitor was set or
            // since the last time this event notice was sent, whichever was last"
            // A 'cleared' state has no value for a delta monitor
            try {
                EVLOG_debug << "Updated monitor: " << monitor_meta.monitor << " reference to: " << value_current;

                if (!this->device_model->update_monitor_reference(monitor_id, value_current)) {
                    EVLOG_warning << "Could not update delta monitor: " << monitor_id << " reference!";
                }
            } catch (const DeviceModelError& e) {
                EVLOG_error << "Could not update delta monitor reference with exception: " << e.what();
            }
        }

        if (it == std::end(updater_monitors_meta)) {
            UpdaterMonitorMeta triggered_meta;

            triggered_meta.type = UpdateMonitorMetaType::TRIGGER;
            triggered_meta.monitor_id = monitor_meta.monitor.id;
            triggered_meta.component = component;
            triggered_meta.variable = variable;
            triggered_meta.monitor_meta = monitor_meta;
            triggered_meta.is_writeonly =
                (attribute.mutability.value_or(MutabilityEnum::ReadWrite) == MutabilityEnum::WriteOnly);

            // Update new current values
            triggered_meta.value_previous = value_previous;
            triggered_meta.value_current = value_current;

            TriggerMetadata metadata;
            metadata.is_csms_sent = 0;
            metadata.is_cleared = 0;
            metadata.is_csms_sent_triggered = 0;
            metadata.is_event_generated = 0;

            triggered_meta.meta_trigger = metadata;

            auto res = updater_monitors_meta.insert(std::pair{monitor_meta.monitor.id, std::move(triggered_meta)});
            if (!res.second) {
                EVLOG_warning << "Could not insert monitor to triggered monitor map!";
                return;
            }
            it = res.first;

            EVLOG_debug << "Variable: " << variable.name.get() << " with monitor: " << monitor_meta.monitor
                        << " triggered, inserted to updater list";
        }

        UpdaterMonitorMeta& triggered_data = it->second;

        // If we are in a 'not dangerous' a.k.a 'cleared' state
        if (triggered_data.meta_trigger.is_cleared && monitor_meta.monitor.type != MonitorEnum::Delta) {
            triggered_data.set_trigger_clear_state(false);

            EVLOG_debug << "Variable: " << variable.name.get()
                        << " triggered already cleared monitor: " << monitor_meta.monitor
                        << ". Setting it back to a 'triggered' state";
        } else if (monitor_meta.monitor.type == MonitorEnum::Delta) {
            // N07.FR.18, N07.FR.19
            // Deltas are always generated when we trigger
            triggered_data.meta_trigger.is_cleared = 0;
            triggered_data.meta_trigger.is_event_generated = 0;

            EVLOG_debug << "Variable: " << variable.name.get() << " triggered delta monitor: " << monitor_meta.monitor
                        << ". Requesting CSMS send";
        }
    } else {
        // If the monitor is not triggered and we already have the data
        // in our triggered list it means that we have returned to normal
        // The return to normal does not apply to 'Delta' monitors
        if (it != std::end(updater_monitors_meta) && monitor_meta.monitor.type != MonitorEnum::Delta) {
            UpdaterMonitorMeta& triggered_data = it->second;
            bool in_triggered_state = (triggered_data.meta_trigger.is_cleared == 0);

            if (in_triggered_state) {
                // Mark it as cleared, a.k.a normal
                triggered_data.set_trigger_clear_state(true);
                EVLOG_debug << "Variable: " << variable.name.get()
                            << " marked monitor as cleared: " << monitor_meta.monitor;
            }
        }
    }
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
        // Evaluate the monitor
        evaluate_monitor(monitor_meta, component, variable, characteristics, attribute, value_previous, value_current);
    }
}

void MonitoringUpdater::update_periodic_monitors_internal() {
    // Update the list of periodic monitors
    auto periodic_monitors = this->device_model->get_periodic_monitors();

    for (auto& component_variable_monitors : periodic_monitors) {
        for (auto& periodic_monitor_meta : component_variable_monitors.monitors) {
            // See if we already have the local monitor
            auto it = this->updater_monitors_meta.find(periodic_monitor_meta.monitor.id);

            if (it != std::end(this->updater_monitors_meta)) {
                // If we already contain it inside, skip
                continue;
            }

            // If it is not found, add a new entry to our managed monitor list
            UpdaterMonitorMeta periodic_meta;

            periodic_meta.type = UpdateMonitorMetaType::PERIODIC;
            periodic_meta.monitor_id = periodic_monitor_meta.monitor.id;
            periodic_meta.component = component_variable_monitors.component;
            periodic_meta.variable = component_variable_monitors.variable;
            periodic_meta.monitor_meta = periodic_monitor_meta;
            periodic_meta.is_writeonly = 0;

            if (periodic_monitor_meta.monitor.type == MonitorEnum::Periodic) {
                // Set the trigger to the current time
                periodic_meta.meta_periodic.last_trigger_steady = std::chrono::steady_clock::now();
            } else if (periodic_monitor_meta.monitor.type == MonitorEnum::PeriodicClockAligned) {
                // Snap to the closest monitor multiple
                periodic_meta.meta_periodic.next_trigger_clock_aligned =
                    get_next_clock_aligned_point(periodic_meta.monitor_meta.monitor.value);
                EVLOG_debug << "First aligned timepoint for monitor ID: " << periodic_monitor_meta.monitor.id;
            } else {
                EVLOG_AND_THROW(std::runtime_error("Invalid type in periodic monitor list, should never happen!"));
            }

            auto res = this->updater_monitors_meta.insert(
                std::pair{periodic_monitor_meta.monitor.id, std::move(periodic_meta)});

            if (!res.second) {
                EVLOG_warning << "Could not insert periodic monitor to internal monitor map!";
                continue;
            }
        }
    }

    // Remove the monitors in our list that don't exist any more in the database
    for (auto it = std::begin(updater_monitors_meta); it != std::end(updater_monitors_meta);) {
        std::int32_t updater_meta_id = it->first;
        auto updater_meta_data = it->second;

        // Ignore triggers
        if (updater_meta_data.type == UpdateMonitorMetaType::TRIGGER) {
            ++it;
            continue;
        }

        bool found_in_new_periodics =
            std::find_if(std::begin(periodic_monitors), std::end(periodic_monitors),
                         [&updater_meta_id](const auto& periodic_monitor) {
                             const auto& monitors = periodic_monitor.monitors;

                             return std::find_if(std::begin(monitors), std::end(monitors),
                                                 [&updater_meta_id](const auto& monitor_meta) {
                                                     return (updater_meta_id == monitor_meta.monitor.id);
                                                 }) != std::end(monitors);
                         }) != std::end(periodic_monitors);

        // If not found, erse from our list as not being relevant
        if (!found_in_new_periodics) {
            it = updater_monitors_meta.erase(it);
        } else {
            ++it;
        }
    }
}

void MonitoringUpdater::process_monitor_meta_internal(UpdaterMonitorMeta& updater_meta_data) {
    const auto& monitor_meta = updater_meta_data.monitor_meta;
    const auto& monitor = monitor_meta.monitor;

    // Process if it is a periodic
    if (updater_meta_data.type == UpdateMonitorMetaType::PERIODIC) {
        // Monitor seconds interval
        auto monitor_seconds =
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<float>(monitor.value));

        // If we match the trigger time
        bool matches_time = false;

        if (monitor.type == MonitorEnum::Periodic) {
            auto current_time = std::chrono::steady_clock::now();
            auto delta = current_time - updater_meta_data.meta_periodic.last_trigger_steady;

            if (delta > monitor_seconds) {
                // Update last time
                updater_meta_data.meta_periodic.last_trigger_steady = current_time;
                matches_time = true;
            }
        } else if (monitor.type == MonitorEnum::PeriodicClockAligned) {
            // 3.55
            // PeriodicClockAligned Triggers an event notice every monitorValue
            // seconds interval, starting from the nearest clock-aligned interval
            // after this monitor was set. For example, a monitorValue of 900 will
            // trigger event notices at 0, 15, 30 and 45 minutes after the hour, every hour.
            auto current_time = std::chrono::system_clock::now();

            if (current_time > updater_meta_data.meta_periodic.next_trigger_clock_aligned) {
                auto distance = std::chrono::duration_cast<std::chrono::seconds>(
                                    current_time - updater_meta_data.meta_periodic.next_trigger_clock_aligned)
                                    .count();

                // Handles with: N08.FR.03, events should be queued and
                // send when the charger is back online
                if (distance > static_cast<decltype(distance)>(60)) {
                    EVLOG_warning << "Missed scheduled monitor time by: " << distance;
                }
                matches_time = true;
                EVLOG_debug << "Reporting periodic monitor with id: " << monitor.id;

                updater_meta_data.meta_periodic.next_trigger_clock_aligned =
                    get_next_clock_aligned_point(monitor.value);
            }
        } else {
            // Should never happen
            EVLOG_AND_THROW(std::runtime_error(std::string("Invalid monitor type from: 'get_periodic_monitors': ") +
                                               conversions::monitor_enum_to_string(monitor.type)));
        }

        if (matches_time) {
            RequiredComponentVariable comp_var;
            comp_var.component = updater_meta_data.component;
            comp_var.variable = updater_meta_data.variable;

            // This operation can cause a small stall, but only if this is triggered
            std::string current_value = this->device_model->get_value<std::string>(comp_var);

            EventData notify_event =
                std::move(create_notify_event(this->unique_id++, current_value, updater_meta_data.component,
                                              updater_meta_data.variable, monitor_meta));

            // Generate one event that will either be sent now, or later based on the offline state
            updater_meta_data.generated_monitor_events.push_back(std::move(notify_event));
        }
    } else if (updater_meta_data.type == UpdateMonitorMetaType::TRIGGER) {
        // If we did not generate an event for this trigger, create the notify event
        if (updater_meta_data.meta_trigger.is_event_generated == 0) {
            // Until next state change, mark this event as generated
            updater_meta_data.meta_trigger.is_event_generated = 1;

            std::string reported_value{};

            // If the variable is marked as read-only then the value will NOT be reported
            if (updater_meta_data.is_writeonly == 0) {
                reported_value = updater_meta_data.value_current;
            }

            EventData notify_event =
                std::move(create_notify_event(unique_id++, reported_value, updater_meta_data.component,
                                              updater_meta_data.variable, updater_meta_data.monitor_meta));

            // N07.FR.18 - the cleared attribute does not apply to deltas
            // N07.FR.19
            if (monitor.type != MonitorEnum::Delta) {
                // Mark if the event is cleared (returned to normal) if that is the case
                notify_event.cleared = (updater_meta_data.meta_trigger.is_cleared == true);
            }

            // Add it to the list of generated events
            updater_meta_data.generated_monitor_events.push_back(std::move(notify_event));
        }
    }
}

bool MonitoringUpdater::should_remove_monitor_meta_internal(const UpdaterMonitorMeta& updater_meta_data) {
    if (updater_meta_data.type == UpdateMonitorMetaType::PERIODIC) {
        return false;
    } else if (updater_meta_data.type == UpdateMonitorMetaType::TRIGGER) {
        bool should_clear = false;

        if ((updater_meta_data.meta_trigger.is_csms_sent_triggered == false) &&
            (updater_meta_data.meta_trigger.is_cleared == true)) {
            // If we never sent to the CSMS a 'trigger' and we are cleared then it means the CSMS
            // does not know of our trigger event, and in case of a return to normal we can simply
            // remove this from the list
            should_clear = true;
        } else if ((updater_meta_data.meta_trigger.is_csms_sent_triggered == true) &&
                   (updater_meta_data.meta_trigger.is_cleared == true) &&
                   (updater_meta_data.meta_trigger.is_csms_sent == true)) {
            // If we sent a 'trigger' to the CSMS but now we are cleared and the current
            // state was also sent to the CSMS it means this trigger can be safely removed
            // as the CSMS knows everything
            should_clear = true;
        }

        return should_clear;
    }

    return false;
}

void MonitoringUpdater::process_monitors_internal(bool allow_periodics, bool allow_trigger) {
    if (!is_monitoring_enabled()) {
        return;
    }

    bool is_offline;
    int offline_severity;
    int active_monitoring_level;
    MonitoringBaseEnum active_monitoring_base;

    get_monitoring_info(is_offline, offline_severity, active_monitoring_level, active_monitoring_base);

    EVLOG_debug << "Processing internal monitors with periodics: " << allow_periodics
                << " and triggers: " << allow_trigger;

    if (allow_periodics) {
        // Rebuild the periodic monitor information
        update_periodic_monitors_internal();
    }

    // Iterate all internal monitors and process them
    for (auto it = std::begin(updater_monitors_meta); it != std::end(updater_monitors_meta);) {
        auto& updater_monitor_meta = it->second;
        const auto& meta_monitor_id = it->first;
        const auto& monitor_meta = updater_monitor_meta.monitor_meta;

        if ((allow_periodics == false) && (updater_monitor_meta.type == UpdateMonitorMetaType::PERIODIC) ||
            (allow_trigger == false) && (updater_monitor_meta.type == UpdateMonitorMetaType::TRIGGER)) {
            continue;
        }

        bool should_process = true;

        // Skip non-active monitors
        if (!is_monitor_active(active_monitoring_base, monitor_meta)) {
            should_process = false;
        }

        if (is_offline) {
            // If we are offline, just discard triggers that have a severity > than 'offline_severity'
            if (it->second.monitor_meta.monitor.severity > offline_severity) {
                should_process = false;
            }
        } else {
            // If we are online, discard the triggers that have a severity > than 'active_monitoring_level'
            if (it->second.monitor_meta.monitor.severity > active_monitoring_level) {
                should_process = false;
            }
        }

        EVLOG_debug << "Monitor: " << updater_monitor_meta.monitor_meta.monitor << " processed: " << should_process;

        if (!should_process) {
            if (updater_monitor_meta.type == UpdateMonitorMetaType::TRIGGER) {
                // The triggers that are not active, should simply pe discarded
                it = updater_monitors_meta.erase(it);
            } else if (updater_monitor_meta.type == UpdateMonitorMetaType::PERIODIC) {
                // Just clear the events, since we don't require them cached
                updater_monitor_meta.generated_monitor_events.clear();
            }

            continue;
        }

        // As a result of this function, the meta should have in it all the generated
        process_monitor_meta_internal(updater_monitor_meta);

        // If we are not offline, send the queued events generated by this meta
        if (!is_offline) {
            if (!updater_monitor_meta.generated_monitor_events.empty()) {
                EVLOG_debug << "Sent data for monitor: " << updater_monitor_meta.monitor_meta.monitor;

                // Send the events
                notify_csms_events(updater_monitor_meta.generated_monitor_events);
                updater_monitor_meta.generated_monitor_events.clear();

                if (updater_monitor_meta.type == UpdateMonitorMetaType::TRIGGER) {
                    // If we have a trigger mark the events as being sent
                    // for the curent state
                    updater_monitor_meta.meta_trigger.is_csms_sent = true;

                    // If this was a state trigger, them also mark that
                    // we sent this 'dangerous' state to the CSMS at least once
                    // since in that case the clear logic changes
                    if (updater_monitor_meta.meta_trigger.is_cleared == false) {
                        updater_monitor_meta.meta_trigger.is_csms_sent_triggered = true;
                    }
                }
            }
        } else {
            // If we are offline but we passed the 'should_process' test, it means that
            // we should keep the generated events and send them at a further occasion
            EVLOG_debug << "We are offline, cached generated events for later!";
        }

        if (should_remove_monitor_meta_internal(updater_monitor_meta)) {
            it = updater_monitors_meta.erase(it);
        } else {
            ++it;
        }
    }
}

bool MonitoringUpdater::is_monitoring_enabled() {
    return this->device_model->get_optional_value<bool>(ControllerComponentVariables::MonitoringCtrlrEnabled)
        .value_or(false);
}

void MonitoringUpdater::get_monitoring_info(bool& out_is_offline, int& out_offline_severity,
                                            int& out_active_monitoring_level,
                                            MonitoringBaseEnum& out_active_monitoring_base) {
    // Persist OfflineMonitoringEventQueuingSeverity even when offline if we have a problem
    out_is_offline = is_chargepoint_offline();

    // By default (if the comp is missing we are reporting up to 'Warning')
    out_offline_severity =
        this->device_model->get_optional_value<int>(ControllerComponentVariables::OfflineQueuingSeverity)
            .value_or(MonitoringLevelSeverity::Warning);

    out_active_monitoring_level =
        this->device_model->get_optional_value<int>(ControllerComponentVariables::ActiveMonitoringLevel)
            .value_or(MonitoringLevelSeverity::MAX);

    std::string active_monitoring_base_string =
        this->device_model->get_optional_value<std::string>(ControllerComponentVariables::ActiveMonitoringBase)
            .value_or(conversions::monitoring_base_enum_to_string(MonitoringBaseEnum::All));

    out_active_monitoring_base = conversions::string_to_monitoring_base_enum(active_monitoring_base_string);
}

} // namespace ocpp::v201