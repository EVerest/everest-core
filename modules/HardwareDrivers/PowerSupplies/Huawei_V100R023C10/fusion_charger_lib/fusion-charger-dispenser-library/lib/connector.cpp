// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "connector.hpp"

#include "fusion_charger/modbus/registers/connector.hpp"

using namespace fusion_charger::modbus_driver::raw_registers;

bool mode_phase_is_export_mode(ModePhase mode_phase) {
    return mode_phase == ModePhase::ExportCableCheck || mode_phase == ModePhase::ExportPrecharge ||
           mode_phase == ModePhase::ExportCharging;
}

ConnectorFSM::ConnectorFSM(ConnectorFSM::Callbacks callbacks, logs::LogIntf log) :
    current_state(States::CarDisconnected), current_mode_phase(ModePhase::Off), callbacks(callbacks), log(log) {
}

void ConnectorFSM::transition(std::optional<States> new_state, std::optional<ModePhase> new_mode_phase) {
    bool state_transitioned = false;
    bool modephase_transitioned = false;

    if (new_state.has_value() && current_state != new_state.value()) {
        log.info << "New state: " + state_to_string(current_state) + " -> " + state_to_string(new_state.value());

        current_state = new_state.value();
        state_transitioned = true;
    }

    if (new_mode_phase.has_value() && current_mode_phase != new_mode_phase.value()) {
        current_mode_phase = new_mode_phase.value();
        modephase_transitioned = true;
    }

    if (!state_transitioned && !modephase_transitioned) {
        return;
    }

    // Call callbacks as needed
    if (state_transitioned) {
        if (callbacks.state_transition.has_value()) {
            callbacks.state_transition.value()(current_state);
        }
    }

    if (modephase_transitioned) {
        if (callbacks.mode_phase_transition.has_value()) {
            callbacks.mode_phase_transition.value()(current_mode_phase);
        }
    }

    if (callbacks.any_transition.has_value()) {
        callbacks.any_transition.value()(current_state, current_mode_phase);
    }
}

States ConnectorFSM::get_state() {
    std::lock_guard<std::mutex> lock(mutex);
    return current_state;
}

ModePhase ConnectorFSM::get_mode_phase() {
    std::lock_guard<std::mutex> lock(mutex);
    return current_mode_phase;
}

void ConnectorFSM::on_car_connected() {
    std::lock_guard<std::mutex> lock(mutex);

    if (current_state == States::CarDisconnected) {
        transition(States::NoKeyYet, std::nullopt);
    }
}

void ConnectorFSM::on_car_disconnected() {
    std::lock_guard<std::mutex> lock(mutex);

    if (current_state != States::CarDisconnected) {
        transition(States::CarDisconnected, std::nullopt);
    }
}

void ConnectorFSM::on_mode_phase_change(ModePhase mode_phase) {
    std::lock_guard<std::mutex> lock(mutex);

    if (current_state == States::Running && mode_phase == ModePhase::Off) {
        transition(States::Completed, mode_phase);
    } else if (current_state == States::Completed && mode_phase_is_export_mode(mode_phase)) {
        transition(States::NoKeyYet, mode_phase);
    } else {
        transition(std::nullopt, mode_phase);
    }
}

void ConnectorFSM::on_module_placeholder_allocation_response(bool success) {
    std::lock_guard<std::mutex> lock(mutex);

    if (current_state == States::ConnectedNoAllocation) {
        // note: we transition to Running even if the allocation failed
        transition(States::Running, std::nullopt);
    }
}

void ConnectorFSM::on_hmac_key_received() {
    std::lock_guard<std::mutex> lock(mutex);

    if (current_state == States::NoKeyYet) {
        transition(States::ConnectedNoAllocation, std::nullopt);
    }
}

WorkingStatus state_to_ws(States state, ModePhase mode_phase) {
    switch (state) {
    case States::CarDisconnected:
        return WorkingStatus::STANDBY;
    case States::NoKeyYet:
        return WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED;
    case States::ConnectedNoAllocation:
        return WorkingStatus::CHARGING_STARTING;
    case States::Running: {
        switch (mode_phase) {
        case ModePhase::Off:
            return WorkingStatus::CHARGING_STARTING;
        case ModePhase::ExportCableCheck:
            return WorkingStatus::CHARGING_STARTING;
        case ModePhase::OffCableCheck:
            return WorkingStatus::CHARGING_STARTING;
        case ModePhase::ExportPrecharge:
            return WorkingStatus::CHARGING_STARTING;
        case ModePhase::ExportCharging:
            return WorkingStatus::CHARGING;
        }
    }
    case States::Completed:
        return WorkingStatus::CHARGING_COMPLETE;
    }

    throw std::runtime_error("Unknown state");
    return WorkingStatus::STANDBY;
}

std::string state_to_string(States state) {
    switch (state) {
    case States::CarDisconnected:
        return "CarDisconnected";
    case States::NoKeyYet:
        return "NoKeyYet";
    case States::ConnectedNoAllocation:
        return "ConnectedNoAllocation";
    case States::Running:
        return "Running";
    case States::Completed:
        return "Completed";
    }

    return "UNKNOWN";
}

Connector::Connector(ConnectorConfig connector_config, uint16_t local_connector_number,
                     DispenserConfig dispenser_config, logs::LogIntf log) :
    connector_config(connector_config),
    local_connector_number(local_connector_number),
    dispenser_config(dispenser_config),
    eth_interface(std::make_shared<goose_ethernet::EthernetInterface>(dispenser_config.eth_interface.c_str())),
    goose_sender(eth_interface, true, log),
    connector_registers_config(ConnectorRegistersConfig{
        .mac_address =
            {
                eth_interface->get_mac_address()[0],
                eth_interface->get_mac_address()[1],
                eth_interface->get_mac_address()[2],
                eth_interface->get_mac_address()[3],
                eth_interface->get_mac_address()[4],
                eth_interface->get_mac_address()[5],
            },
        .type = connector_config.connector_type,
        .global_connector_no = connector_config.global_connector_number,
        .connector_number = local_connector_number,
        .max_rated_charge_current = connector_config.max_rated_charge_current,
        .rated_output_power_connector = connector_config.max_rated_output_power / 1000,
        .get_contactor_upstream_voltage = connector_config.connector_callbacks.connector_upstream_voltage,
        .get_output_voltage = connector_config.connector_callbacks.output_voltage,
        .get_output_current = connector_config.connector_callbacks.output_current,
        .get_contactor_status = connector_config.connector_callbacks.contactor_status,
        .get_electronic_lock_status = connector_config.connector_callbacks.electronic_lock_status,
    }),
    connector_registers(connector_registers_config),
    log(log),
    fsm(
        {
            .state_transition = std::bind(&Connector::on_state_transition, this, std::placeholders::_1),
            .any_transition = std::bind(&Connector::on_state_mode_phase_transition, this, std::placeholders::_1,
                                        std::placeholders::_2),
        },
        log) {
}

Connector::~Connector() {
    cancel_module_placeholder_allocation_timeout();
}

void Connector::on_state_transition(States state) {
    // Update Connector event
    if (state == States::Running) {
        connector_registers.charging_event_connector.report(
            CollectedConnectorRegisters::ChargingEventConnector::STOP_TO_START);
    } else {
        connector_registers.charging_event_connector.report(
            CollectedConnectorRegisters::ChargingEventConnector::START_TO_STOP);
    }

    // Update connection status
    if (state == States::CarDisconnected) {
        set_connection_status(ConnectionStatus::NOT_CONNECTED);
    } else {
        set_connection_status(ConnectionStatus::FULL_CONNECTED);
    }

    // Module placeholder allocation timeout
    if (state == States::ConnectedNoAllocation) {
        cancel_module_placeholder_allocation_timeout();

        // start timeout thread
        log.verbose << "Starting module placeholder allocation timeout thread";
        module_placeholder_allocation_timeout.thread = std::thread([this]() {
            std::cv_status wait_resp;
            {
                std::unique_lock<std::mutex> lock(module_placeholder_allocation_timeout.received_mutex);
                wait_resp = module_placeholder_allocation_timeout.received_cv.wait_for(
                    lock, dispenser_config.module_placeholder_allocation_timeout);
            }

            if (wait_resp == std::cv_status::no_timeout) {
                return;
            }

            log.verbose << "MPAC Timeout thread: timeout reached, checking state";
            if (fsm.get_state() == States::ConnectedNoAllocation) {
                on_module_placeholder_allocation_timeout();
            }
        });
    }

    if (state != States::Running) {
        // The new states after a transition from the Running state are either
        // Completed or CarDisconnected, thus this is the perfect
        // time to do it
        last_module_placeholder_allocation_failed = false;
    }
}

void Connector::on_state_mode_phase_transition(States state, ModePhase mode_phase) {
    auto ws = state_to_ws(state, mode_phase);
    set_working_status(ws);

    send_needed_goose_frame(state, mode_phase);
}

void Connector::start() {
    last_module_placeholder_allocation_failed = false;
    // Re-Init Connector Registers to reset them on every start
    connector_registers = ConnectorRegisters(connector_registers_config),

    connector_registers.hmac_key.add_write_callback([this](const uint8_t* value) {
        char hmac_str[97];
        for (int i = 0; i < 48; i++) {
            sprintf(hmac_str + i * 2, "%02X", value[i]);
        }
        log.info << "🔑 Connector HMAC key changed to " + std::string(hmac_str);

        goose_sender.on_new_hmac_key(std::vector<uint8_t>(value, value + 48));

        fsm.on_hmac_key_received();
    });

    connector_registers.psu_port_available.add_write_callback([this](PsuOutputPortAvailability value) {
        log.info << "PSU port available changed to " + std::to_string((uint16_t)value);
    });

    connector_registers.rated_output_power_psu.add_write_callback(
        [this](float value) { log.info << "Rated output power PSU changed to " + std::to_string(value); });

    // todo: reset fsm?

    goose_sender.start();
}

void Connector::stop() {
    goose_sender.stop();
    cancel_module_placeholder_allocation_timeout();
}

bool Connector::module_placeholder_allocation_failed() {
    return last_module_placeholder_allocation_failed;
}

PsuOutputPortAvailability Connector::get_output_port_availability() {
    return connector_registers.psu_port_available.get_value();
}

Capabilities Connector::get_capabilities() {
    return Capabilities{.max_export_voltage_V = connector_registers.max_rated_psu_voltage.get_value(),
                        .min_export_voltage_V = connector_registers.min_rated_psu_voltage.get_value(),
                        .max_export_current_A = connector_registers.max_rated_psu_current.get_value(),
                        .min_export_current_A = connector_registers.min_rated_psu_current.get_value(),
                        .max_export_power_W = connector_registers.rated_output_power_psu.get_value() * 1000};
}

void Connector::set_total_historical_energy_charged_per_connector(double energy_charged) {
    connector_registers.total_energy_charged.update_value(energy_charged);
}

WorkingStatus Connector::get_working_status() {
    return connector_registers.working_status.get_value();
}

void Connector::on_car_connected() {
    fsm.on_car_connected();
}

void Connector::on_car_disconnected() {
    fsm.on_car_disconnected();
}

void Connector::on_mode_phase_change(ModePhase mode_phase) {
    fsm.on_mode_phase_change(mode_phase);
}

void Connector::new_export_voltage_current(double voltage, double current) {
    this->current_requested_voltage_current.voltage = voltage;
    this->current_requested_voltage_current.current = current;

    if (fsm.get_state() == States::Running) {
        send_needed_goose_frame();
    }
}

void Connector::send_needed_goose_frame() {
    send_needed_goose_frame(fsm.get_state(), fsm.get_mode_phase());
}
void Connector::send_needed_goose_frame(States state, ModePhase mode_phase) {
    switch (state) {
    case States::CarDisconnected:
    case States::NoKeyYet:
        goose_sender.send_stop_request(connector_config.global_connector_number);
        break;
    case States::ConnectedNoAllocation:
        goose_sender.send_module_placeholder_request(connector_config.global_connector_number);
        break;

    case States::Running: {
        switch (mode_phase) {
        case ModePhase::Off:
            // as we are in the running state, we have to continue charging
            // stuff, not stopping stuff. If we get a new ModePhase::Off we
            // then have to stop charging (goes to States::Completed
            // immediately through on_mode_phase_change) thus we continue to send
            // MPRs until we get another ModePhase
            goose_sender.send_module_placeholder_request(connector_config.global_connector_number);
            break;
        case ModePhase::ExportCableCheck:
            goose_sender.send_insulation_detection_voltage_output(connector_config.global_connector_number,
                                                                  current_requested_voltage_current.voltage,
                                                                  current_requested_voltage_current.current);
            break;
        case ModePhase::OffCableCheck:
            goose_sender.send_insulation_detection_voltage_output_stoppage(connector_config.global_connector_number);
            break;
        case ModePhase::ExportPrecharge:
            goose_sender.send_precharge_voltage_output(connector_config.global_connector_number,
                                                       current_requested_voltage_current.voltage,
                                                       current_requested_voltage_current.current);
            break;
        case ModePhase::ExportCharging:
            // initial send of charging power requirement; more will be sent
            // upon new_export_voltage_current
            goose_sender.send_charging_voltage_output(connector_config.global_connector_number,
                                                      current_requested_voltage_current.voltage,
                                                      current_requested_voltage_current.current);
            break;
        }
        break;
    }
    case States::Completed:
        goose_sender.send_stop_request(connector_config.global_connector_number);
        break;
    }
}

void Connector::set_working_status(WorkingStatus status) {
    log.info << "Set working status for connector " + std::to_string(local_connector_number) +
                    " to: " + working_status_to_string(status);

    connector_registers.working_status.update_value(status);
}

void Connector::set_connection_status(ConnectionStatus status) {
    connector_registers.connection_status.update_value(status);
}

void Connector::on_module_placeholder_allocation_response(bool success) {
    if (success) {
        log.info << "Module placeholder allocation received response, SUCCESS";
    } else {
        log.info << "Module placeholder allocation received response, FAILED";
    }

    if (fsm.get_state() == States::ConnectedNoAllocation) {
        last_module_placeholder_allocation_failed = !success;
    }

    cancel_module_placeholder_allocation_timeout();

    fsm.on_module_placeholder_allocation_response(success);
}

void Connector::on_module_placeholder_allocation_timeout() {
    log.warning << "Module placeholder allocation timeout";

    this->last_module_placeholder_allocation_failed = true;
    fsm.on_module_placeholder_allocation_response(false);
}

void Connector::cancel_module_placeholder_allocation_timeout() {
    if (this->module_placeholder_allocation_timeout.thread.joinable()) {
        log.verbose << "Cancelling module placeholder allocation timeout thread";

        {
            std::lock_guard<std::mutex> lock(module_placeholder_allocation_timeout.received_mutex);
            module_placeholder_allocation_timeout.received_cv.notify_all();
        }
        this->module_placeholder_allocation_timeout.thread.join();
    }
}

void Connector::car_connect_disconnect_cycle(std::chrono::milliseconds timeout) {
    auto mode_phase_before = fsm.get_mode_phase();
    fsm.on_mode_phase_change(ModePhase::Off);

    auto timeout_timestamp = std::chrono::steady_clock::now() + timeout;

    on_car_connected();
    // Wait until we got the hmac key
    while (fsm.get_state() != States::ConnectedNoAllocation && fsm.get_state() != States::Running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (std::chrono::steady_clock::now() > timeout_timestamp) {
            log.error << "Timeout while waiting for the hmac key";
            break;
        }
    }
    on_car_disconnected();

    // If in the time we did car connect - disconnect cycle the mode/phase didnt
    // change go back to the previous mode/phase
    if (fsm.get_mode_phase() == ModePhase::Off) {
        fsm.on_mode_phase_change(mode_phase_before);
    }
}

void Connector::on_psu_mac_change(std::vector<uint8_t> mac_address) {
    this->goose_sender.on_new_mac_address(mac_address);
    this->send_needed_goose_frame();
}
