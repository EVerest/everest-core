// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "base.hpp"
#include <algorithm>
#include <chrono>

namespace module {

ConnectorBase::ConnectorBase(std::uint8_t connector, power_supply_DCImplBase* impl) :
    connector_no(connector), impl(impl), log_prefix("Connector #" + std::to_string(connector + 1) + ": ") {
}

void ConnectorBase::ev_set_config(EverestConnectorConfig config) {
    this->config = config;
}

void ConnectorBase::ev_set_mod(const Everest::PtrContainer<Huawei_V100R023C10>& mod) {
    this->mod = mod;
}

void ConnectorBase::do_init_hmac_acquire() {
    std::lock_guard lock(connector_mutex);

    EVLOG_info << log_prefix << "Trying to acquire hmac key to stop charging if it is still running";
    this->get_connector()->car_connect_disconnect_cycle(std::chrono::seconds(10));
    EVLOG_info << log_prefix << "Acquired hmac key";
}

ConnectorConfig ConnectorBase::get_connector_config() {
    ConnectorConfig connector_config;

    connector_config.global_connector_number = (std::uint16_t)config.global_connector_number;
    connector_config.connector_type = ConnectorType::CCS2;
    connector_config.max_rated_charge_current = (float)config.max_export_current_A;
    connector_config.max_rated_output_power = (float)config.max_export_power_W;

    ConnectorCallbacks connector_callbacks;

    connector_callbacks.connector_upstream_voltage = [this]() -> float {
        return this->external_provided_data.upstream_voltage.load();
    };
    connector_callbacks.output_voltage = [this]() -> float {
        return this->external_provided_data.output_voltage.load();
    };
    connector_callbacks.output_current = [this]() -> float {
        return this->external_provided_data.output_current.load();
    };
    connector_callbacks.contactor_status = [this]() -> ContactorStatus {
        return this->external_provided_data.contactor_status.load();
    };

    // we just return LOCKED as default value
    connector_callbacks.electronic_lock_status = [this]() -> ElectronicLockStatus {
        return ElectronicLockStatus::LOCKED;
    };
    connector_config.connector_callbacks = connector_callbacks;
    return connector_config;
}

void ConnectorBase::ev_init() {
    if (config.global_connector_number < 0) {
        EVLOG_critical << log_prefix << ": initialized but global connector number is invalid";
        throw std::runtime_error("Invalid global connector number");
    }

    init_capabilities();

    telemetry_subtopic = "connector/" + std::to_string(config.global_connector_number) + "/dispenser_to_psu";
    mod->telemetry_publisher->add_subtopic(telemetry_subtopic);

    mod->telemetry_publisher->initialize_datapoint(telemetry_subtopic, telemetry_datapoint_keys::BSP_EVENT);

    mod->r_board_support[this->connector_no]->subscribe_event(
        [this](const types::board_support_common::BspEvent& event) {
            EVLOG_verbose << log_prefix << "Received event: " << event;

            if (event.event == types::board_support_common::Event::PowerOn) {
                this->external_provided_data.contactor_status = ContactorStatus::ON;
            } else if (event.event == types::board_support_common::Event::PowerOff) {
                this->external_provided_data.contactor_status = ContactorStatus::OFF;
            }

            mod->telemetry_publisher->datapoint_changed(telemetry_subtopic, telemetry_datapoint_keys::BSP_EVENT,
                                                        types::board_support_common::event_to_string(event.event));

            auto connector = this->get_connector();
            std::lock_guard lock(connector_mutex);

            if (event.event == types::board_support_common::Event::A) {
                connector->on_car_disconnected();
            } else if (event.event == types::board_support_common::Event::B) {
                connector->on_car_connected();
            }
        });

    mod->telemetry_publisher->initialize_datapoint(telemetry_subtopic, telemetry_datapoint_keys::OUTPUT_VOLTAGE);
    mod->telemetry_publisher->initialize_datapoint(telemetry_subtopic, telemetry_datapoint_keys::OUTPUT_CURRENT);

    if (not mod->r_carside_powermeter.empty()) {
        mod->r_carside_powermeter[this->connector_no]->subscribe_powermeter(
            [this](const types::powermeter::Powermeter& power) {
                EVLOG_verbose << log_prefix << "Received powermeter measurement: " << power;

                auto output_voltage = power.voltage_V.value_or(types::units::Voltage{.DC = 0}).DC.value_or(0);
                auto output_current = power.current_A.value_or(types::units::Current{.DC = 0}).DC.value_or(0);

                this->external_provided_data.output_voltage = output_voltage;
                this->external_provided_data.output_current = output_current;

                if (mod->config.HACK_use_ovm_while_cable_check and
                    last_phase == types::power_supply_DC::ChargingPhase::CableCheck) {
                    return; // do not publish the powermeter values during cable check phase when HACK is enabled
                }

                types::power_supply_DC::VoltageCurrent export_vc = {
                    .voltage_V = output_voltage,
                    .current_A = output_current,
                };

                EVLOG_debug << log_prefix << "Publishing voltage/current from powermeter: " << output_voltage << "V "
                            << output_current << "A";

                // Everest voltage measurement publishing
                this->impl->publish_voltage_current(export_vc);

                mod->telemetry_publisher->datapoint_changed(telemetry_subtopic,
                                                            telemetry_datapoint_keys::OUTPUT_VOLTAGE, output_voltage);
                mod->telemetry_publisher->datapoint_changed(telemetry_subtopic,
                                                            telemetry_datapoint_keys::OUTPUT_CURRENT, output_current);
            });
    }

    mod->telemetry_publisher->initialize_datapoint(telemetry_subtopic, telemetry_datapoint_keys::UPSTREAM_VOLTAGE);

    // note that Huawei_V100R023C10 already checks, if the required interfaces are available
    if (mod->upstream_voltage_source == Huawei_V100R023C10::UpstreamVoltageSource::IMD) {
        mod->r_isolation_monitor[this->connector_no]->subscribe_isolation_measurement(
            [this](const types::isolation_monitor::IsolationMeasurement& iso) {
                EVLOG_verbose << log_prefix << "Received isolation measurement: " << iso;

                // Upstream voltage publishing
                EVLOG_debug << log_prefix << "Publishing upstream voltage from IMD: " << iso.voltage_V.value_or(0)
                            << "V";
                this->external_provided_data.upstream_voltage = iso.voltage_V.value_or(0);
                mod->telemetry_publisher->datapoint_changed(telemetry_subtopic,
                                                            telemetry_datapoint_keys::UPSTREAM_VOLTAGE,
                                                            this->external_provided_data.upstream_voltage);
            });
    }

    // note that Huawei_V100R023C10 already checks, if the required interfaces are available
    if (mod->upstream_voltage_source == Huawei_V100R023C10::UpstreamVoltageSource::OVM or
        mod->config.HACK_use_ovm_while_cable_check) {
        mod->r_over_voltage_monitor[this->connector_no]->subscribe_voltage_measurement_V([this](double voltage) {
            EVLOG_verbose << log_prefix << "Received OVM voltage measurement: " << voltage << "V";

            // Upstream voltage publishing
            if (mod->upstream_voltage_source == Huawei_V100R023C10::UpstreamVoltageSource::OVM) {
                EVLOG_debug << log_prefix << "Publishing upstream voltage from OVM: " << voltage << "V";
                this->external_provided_data.upstream_voltage = voltage;
                mod->telemetry_publisher->datapoint_changed(telemetry_subtopic,
                                                            telemetry_datapoint_keys::UPSTREAM_VOLTAGE,
                                                            this->external_provided_data.upstream_voltage);
            }

            // Everest voltage measurement publishing
            // only publish the ovm values if we are in the cable check phase
            if (mod->config.HACK_use_ovm_while_cable_check and
                last_phase == types::power_supply_DC::ChargingPhase::CableCheck) {

                EVLOG_debug << log_prefix << "Publishing voltage/current from OVM: " << voltage << "V 0A";

                types::power_supply_DC::VoltageCurrent export_vc = {
                    .voltage_V = (float)voltage,
                    .current_A = 0,
                };

                this->impl->publish_voltage_current(export_vc);
            }
        });
    }

    mod->telemetry_publisher->initialize_datapoint(telemetry_subtopic, telemetry_datapoint_keys::EVEREST_MODE);
    mod->telemetry_publisher->initialize_datapoint(telemetry_subtopic, telemetry_datapoint_keys::EVEREST_PHASE);
    mod->telemetry_publisher->initialize_datapoint(telemetry_subtopic, telemetry_datapoint_keys::EXPORT_VOLTAGE);
    mod->telemetry_publisher->initialize_datapoint(telemetry_subtopic, telemetry_datapoint_keys::EXPORT_CURRENT);
}

void ConnectorBase::ev_ready() {
    capabilities_not_received_raised = true;
    raise_missing_capabilities_error();

    this->worker_thread_handle = std::thread(std::bind(&ConnectorBase::worker_thread, this));

    this->impl->publish_capabilities(caps);
    this->impl->publish_mode(types::power_supply_DC::Mode::Off);
}

void ConnectorBase::ev_handle_setMode(types::power_supply_DC::Mode mode, types::power_supply_DC::ChargingPhase phase) {

    // if we get the stop request after cable check, we keep the phase
    if (last_mode == types::power_supply_DC::Mode::Export && mode == types::power_supply_DC::Mode::Off &&
        last_phase == types::power_supply_DC::ChargingPhase::CableCheck) {
        phase = types::power_supply_DC::ChargingPhase::CableCheck;
    }

    EVLOG_debug << log_prefix << "Setting mode to " << mode << " and phase to " << phase;

    last_mode = mode;
    last_phase = phase;

    std::lock_guard lock(connector_mutex);

    switch (mode) {
    case types::power_supply_DC::Mode::Off:
        switch (phase) {
        case types::power_supply_DC::ChargingPhase::CableCheck:
            this->get_connector()->on_mode_phase_change(ModePhase::OffCableCheck);
            break;
        default:
            this->get_connector()->on_mode_phase_change(ModePhase::Off);
            break;
        }
        break;
    case types::power_supply_DC::Mode::Export:
        switch (phase) {
        case types::power_supply_DC::ChargingPhase::CableCheck:
            this->get_connector()->on_mode_phase_change(ModePhase::ExportCableCheck);
            break;
        case types::power_supply_DC::ChargingPhase::PreCharge:
            this->get_connector()->on_mode_phase_change(ModePhase::ExportPrecharge);
            break;
        case types::power_supply_DC::ChargingPhase::Charging:
            this->get_connector()->on_mode_phase_change(ModePhase::ExportCharging);
            break;
        default:
            EVLOG_info << log_prefix << "Unknown Export phase: " << phase;
            break;
        }
        break;

    default:
        break;
    }

    mod->telemetry_publisher->datapoint_changed(telemetry_subtopic, telemetry_datapoint_keys::EVEREST_MODE,
                                                types::power_supply_DC::mode_to_string(mode));
    mod->telemetry_publisher->datapoint_changed(telemetry_subtopic, telemetry_datapoint_keys::EVEREST_PHASE,
                                                types::power_supply_DC::charging_phase_to_string(phase));

    this->impl->publish_mode(mode);
}
void ConnectorBase::ev_handle_setExportVoltageCurrent(double voltage, double current) {
    EVLOG_debug << log_prefix << "Setting export voltage to " << voltage << "V and current to " << current << "A";
    if (voltage > caps.max_export_voltage_V)
        voltage = caps.max_export_voltage_V;
    else if (voltage < caps.min_export_voltage_V)
        voltage = caps.min_export_voltage_V;

    if (current > caps.max_export_current_A)
        current = caps.max_export_current_A;
    else if (current < caps.min_export_current_A)
        current = caps.min_export_current_A;

    std::lock_guard lock(connector_mutex);

    export_voltage = voltage;
    export_current_limit = current;

    mod->telemetry_publisher->datapoint_changed(telemetry_subtopic, telemetry_datapoint_keys::EXPORT_VOLTAGE, voltage);
    mod->telemetry_publisher->datapoint_changed(telemetry_subtopic, telemetry_datapoint_keys::EXPORT_CURRENT, current);

    this->get_connector()->new_export_voltage_current(voltage, current);
}
void ConnectorBase::ev_handle_setImportVoltageCurrent(double voltage, double current) {
    EVLOG_error << "Not implemented";
}

void ConnectorBase::worker_thread() {
    for (;;) {
        update_module_placeholder_errors();
        update_hack();
        update_and_publish_capabilities();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void ConnectorBase::update_module_placeholder_errors() {
    if (this->get_connector()->module_placeholder_allocation_failed()) {
        this->raise_module_placeholder_allocation_failure();
    } else {
        this->clear_module_placeholder_allocation_failure();
    }
}

void ConnectorBase::update_hack() {
    if (this->mod->config.HACK_publish_requested_voltage_current) {
        types::power_supply_DC::VoltageCurrent export_vc;
        export_vc.voltage_V = (float)this->export_voltage;
        export_vc.current_A = (float)this->export_current_limit;
        if (this->last_mode == types::power_supply_DC::Mode::Off) {
            export_vc.voltage_V = 0;
            export_vc.current_A = 0;
        }
        this->impl->publish_voltage_current(export_vc);
    }
}

void ConnectorBase::update_and_publish_capabilities() {
    auto new_caps = this->get_connector()->get_capabilities();

    // apply config limits to the psu capabilities
    new_caps.max_export_current_A =
        std::min(static_cast<double>(new_caps.max_export_current_A), config.max_export_current_A);
    new_caps.max_export_power_W = std::min(static_cast<double>(new_caps.max_export_power_W), config.max_export_power_W);

    std::lock_guard lock(connector_mutex);

    bool caps_changed = false;

    if (caps.max_export_voltage_V != new_caps.max_export_voltage_V) {
        caps.max_export_voltage_V = new_caps.max_export_voltage_V;
        caps_changed = true;
    }
    if (caps.min_export_voltage_V != new_caps.min_export_voltage_V) {
        caps.min_export_voltage_V = new_caps.min_export_voltage_V;
        caps_changed = true;
    }
    if (caps.max_export_current_A != new_caps.max_export_current_A) {
        caps.max_export_current_A = new_caps.max_export_current_A;
        caps_changed = true;
    }
    if (caps.min_export_current_A != new_caps.min_export_current_A) {
        caps.min_export_current_A = new_caps.min_export_current_A;
        caps_changed = true;
    }
    if (caps.max_export_power_W != new_caps.max_export_power_W) {
        caps.max_export_power_W = new_caps.max_export_power_W;
        caps_changed = true;
    }

    if (caps_changed) {
        EVLOG_info << log_prefix << "Updating capabilities";
        this->impl->publish_capabilities(caps);

        if (capabilities_not_received_raised and caps.max_export_current_A > 0 and caps.max_export_voltage_V > 0 and
            caps.max_export_power_W > 0) {
            capabilities_not_received_raised = false;
            this->clear_missing_capabilities_error();
        }
    }
}

void ConnectorBase::raise_missing_capabilities_error() {
    this->impl->raise_error(this->impl->error_factory->create_error(
        "power_supply_DC/HardwareFault", "capabilities_not_received", "", Everest::error::Severity::High));
}
void ConnectorBase::clear_missing_capabilities_error() {
    this->impl->clear_error("power_supply_DC/HardwareFault", "capabilities_not_received");
}

void ConnectorBase::init_capabilities() {
    caps.current_regulation_tolerance_A = 1;
    caps.peak_current_ripple_A = 0.5;

    caps.min_export_current_A = 0;
    caps.max_export_current_A = 0;
    caps.min_export_voltage_V = 0;
    caps.max_export_voltage_V = 0;
    caps.max_export_power_W = 0;

    caps.max_import_current_A = 0;
    caps.min_import_current_A = 0;
    caps.max_import_power_W = 0;
    caps.min_import_voltage_V = 0;
    caps.max_import_voltage_V = 0;

    caps.conversion_efficiency_import = 0.85f;
    caps.conversion_efficiency_export = 0.9f;

    caps.bidirectional = false;
}

void ConnectorBase::raise_communication_fault() {
    this->impl->raise_error(this->impl->error_factory->create_error(
        "power_supply_DC/CommunicationFault", "", "Communication error", Everest::error::Severity::High));
}
void ConnectorBase::clear_communication_fault() {
    this->impl->clear_error("power_supply_DC/CommunicationFault");
}

void ConnectorBase::raise_psu_not_running() {
    this->impl->raise_error(this->impl->error_factory->create_error("power_supply_DC/HardwareFault", "psu_not_running",
                                                                    "", Everest::error::Severity::High));
}
void ConnectorBase::clear_psu_not_running() {
    this->impl->clear_error("power_supply_DC/HardwareFault", "psu_not_running");
}

void ConnectorBase::raise_module_placeholder_allocation_failure() {
    if (!module_placeholder_allocation_failure_raised) {
        this->impl->raise_error(this->impl->error_factory->create_error("power_supply_DC/VendorWarning",
                                                                        "module_placeholder_allocation_failed", "",
                                                                        Everest::error::Severity::High));
        module_placeholder_allocation_failure_raised = true;
    }
}
void ConnectorBase::clear_module_placeholder_allocation_failure() {
    if (module_placeholder_allocation_failure_raised) {
        this->impl->clear_error("power_supply_DC/VendorWarning", "module_placeholder_allocation_failed");
        module_placeholder_allocation_failure_raised = false;
    }
}

Connector* ConnectorBase::get_connector() {
    return this->mod->dispenser->get_connector(this->connector_no + 1).get();
}

void ConnectorBase::raise_psu_error(ErrorEvent error) {
    this->impl->raise_error(
        this->impl->error_factory->create_error("power_supply_DC/VendorWarning", error.to_everest_subtype(),
                                                error.to_error_log_string(), Everest::error::Severity::Medium));
}

void ConnectorBase::clear_psu_error(ErrorEvent error) {
    this->impl->clear_error("power_supply_DC/VendorWarning", error.to_everest_subtype());
}

void ConnectorBase::clear_stored_capabilities() {
    std::lock_guard lock(connector_mutex);
    // If the error is not raised yet, raise it and clear the capabilities until we get them again
    if (not capabilities_not_received_raised) {
        capabilities_not_received_raised = true;
        init_capabilities();
        this->get_connector()->reset_psu_capabilities();
        this->impl->publish_capabilities(caps);
        raise_missing_capabilities_error();
    }
}

}; // namespace module
