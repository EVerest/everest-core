// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "base.hpp"
#include <algorithm>
#include <chrono>

namespace module {

ConnectorBase::ConnectorBase(uint8_t connector, power_supply_DCImplBase* impl) : connector_no(connector), impl(impl) {
}

void ConnectorBase::ev_set_config(EverestConnectorConfig config) {
    this->config = config;
}

void ConnectorBase::ev_set_mod(const Everest::PtrContainer<Huawei_V100R023C10>& mod) {
    this->mod = mod;
}

void ConnectorBase::do_init_hmac_acquire() {
    std::lock_guard lock(connector_mutex);

    EVLOG_info << "Trying to acquire hmac key to stop charging if it is still running";
    this->get_connector()->car_connect_disconnect_cycle(std::chrono::seconds(10));
    EVLOG_info << "Acquired hmac key";
}

ConnectorConfig ConnectorBase::get_connector_config() {
    return {
        .global_connector_number = (uint16_t)config.global_connector_number,
        .connector_type = ConnectorType::CCS2,
        .max_rated_charge_current = (float)config.max_export_current_A,
        .max_rated_output_power = (float)config.max_export_power_W,

        .connector_callbacks = {
            .connector_upstream_voltage = [this]() -> float {
                return this->external_provided_data.upstream_voltage.load();
            },
            .output_voltage = [this]() -> float { return this->external_provided_data.output_voltage.load(); },
            .output_current = [this]() -> float { return this->external_provided_data.output_current.load(); },
            .contactor_status = [this]() -> ContactorStatus {
                return this->external_provided_data.contactor_status.load();
            },

            // we just return LOCKED as default value
            .electronic_lock_status = [this]() -> ElectronicLockStatus { return ElectronicLockStatus::LOCKED; },
        },
    };
}

void ConnectorBase::ev_init() {
    if (config.global_connector_number < 0) {
        EVLOG_critical << "Connector " + std::to_string(connector_no) +
                              " initialized but global connector number is invalid";
        throw std::runtime_error("Invalid global connector number");
    }

    init_capabilities();

    mod->r_board_support[this->connector_no]->subscribe_event(
        [this](const types::board_support_common::BspEvent& event) {
            EVLOG_verbose << "Received event: " << event;

            if (event.event == types::board_support_common::Event::PowerOn) {
                this->external_provided_data.contactor_status = ContactorStatus::ON;
            } else if (event.event == types::board_support_common::Event::PowerOff) {
                this->external_provided_data.contactor_status = ContactorStatus::OFF;
            }

            auto connector = this->get_connector();
            std::lock_guard lock(connector_mutex);

            if (event.event == types::board_support_common::Event::A) {
                connector->on_car_disconnected();
            } else if (event.event == types::board_support_common::Event::B) {
                connector->on_car_connected();
            }
        });

    mod->r_isolation_monitor[this->connector_no]->subscribe_isolation_measurement(
        [this](const types::isolation_monitor::IsolationMeasurement& iso) {
            EVLOG_verbose << "Received isolation measurement: " << iso;

            this->external_provided_data.upstream_voltage = iso.voltage_V.value_or(0);
        });

    if (!mod->r_carside_powermeter.empty()) {
        mod->r_carside_powermeter[this->connector_no]->subscribe_powermeter(
            [this](const types::powermeter::Powermeter& power) {
                EVLOG_verbose << "Received powermeter measurement: " << power;

                auto output_voltage = power.voltage_V.value_or(types::units::Voltage{.DC = 0}).DC.value_or(0);
                auto output_current = power.current_A.value_or(types::units::Current{.DC = 0}).DC.value_or(0);

                this->external_provided_data.output_voltage = output_voltage;
                this->external_provided_data.output_current = output_current;

                types::power_supply_DC::VoltageCurrent export_vc = {
                    .voltage_V = output_voltage,
                    .current_A = output_current,
                };

                this->impl->publish_voltage_current(export_vc);
            });
    }
}

void ConnectorBase::ev_ready() {
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

    EVLOG_info << "Setting mode to " << mode << " and phase to " << phase;

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
            EVLOG_info << "Unknown Export phase: " << phase;
            break;
        }
        break;

    default:
        break;
    }

    this->impl->publish_mode(mode);
}
void ConnectorBase::ev_handle_setExportVoltageCurrent(double voltage, double current) {
    EVLOG_info << "Setting export voltage to " << voltage << "V and current to " << current << "A";
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
        auto export_vc = types::power_supply_DC::VoltageCurrent{
            .voltage_V = (float)this->export_voltage,
            .current_A = (float)this->export_current_limit,
        };
        if (this->last_mode == types::power_supply_DC::Mode::Off) {
            export_vc.voltage_V = 0;
            export_vc.current_A = 0;
        }
        this->impl->publish_voltage_current(export_vc);
    }
}

void ConnectorBase::update_and_publish_capabilities() {
    auto new_caps = this->get_connector()->get_capabilities();

    std::lock_guard lock(connector_mutex);

    // todo: taken from old module; might need to update this...
    if (caps.max_export_power_W != new_caps.max_export_power_W) {
        caps.max_export_power_W = new_caps.max_export_power_W;
        // todo: how about the other caps?

        this->impl->publish_capabilities(caps);
    }
}

void ConnectorBase::init_capabilities() {
    caps.current_regulation_tolerance_A = 1;
    caps.peak_current_ripple_A = 0.5;

    caps.min_export_current_A = 0;
    caps.max_export_current_A = config.max_export_current_A;
    caps.min_export_voltage_V = 150;
    caps.max_export_voltage_V = 1000;
    caps.max_export_power_W = 0;

    caps.max_import_current_A = 0;
    caps.min_import_current_A = 0;
    caps.max_import_power_W = 0;
    caps.min_import_voltage_V = 0;
    caps.max_import_voltage_V = 0;

    caps.conversion_efficiency_import = 0.85f;
    caps.conversion_efficiency_export = 0.9f;
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
        this->impl->raise_error(this->impl->error_factory->create_error(
            "power_supply_DC/VendorError", "module_placeholder_allocation_failed", "", Everest::error::Severity::High));
        module_placeholder_allocation_failure_raised = true;
    }
}
void ConnectorBase::clear_module_placeholder_allocation_failure() {
    if (module_placeholder_allocation_failure_raised) {
        this->impl->clear_error("power_supply_DC/VendorError", "module_placeholder_allocation_failed");
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

}; // namespace module
