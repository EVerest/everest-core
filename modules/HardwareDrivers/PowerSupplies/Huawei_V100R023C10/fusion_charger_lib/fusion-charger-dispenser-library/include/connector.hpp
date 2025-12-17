// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <fusion_charger/modbus/registers/connector.hpp>
#include <mutex>

#include "callbacks.hpp"
#include "configuration.hpp"
#include "connector_goose_sender.hpp"
#include "logs/logs.hpp"

struct Capabilities {
    float max_export_voltage_V;
    float min_export_voltage_V;
    float max_export_current_A;
    float min_export_current_A;
    float max_export_power_W;
};

enum class ModePhase {
    Off,
    ExportCableCheck,
    OffCableCheck,
    ExportPrecharge,
    ExportCharging,
};

enum class States {
    CarDisconnected,
    NoKeyYet,
    ConnectedNoAllocation,
    Running,
    Completed
};

fusion_charger::modbus_driver::raw_registers::WorkingStatus state_to_ws(States state, ModePhase mode_phase);
std::string state_to_string(States state);

/**
 * @brief Returns true if the mode phase is an export mode
 *
 * @param mode_phase the mode phase
 * @return true if the mode phase is ExportCableCheck, ExportPrecharge or
 * ExportCharging
 * @return false otherwise
 */
bool mode_phase_is_export_mode(ModePhase mode_phase);

class ConnectorFSM {
    States current_state;
    ModePhase current_mode_phase;

    logs::LogIntf log;
    std::mutex mutex;

    std::string log_prefix; // Prefix for log messages, can be set in the constructor

    /**
     * @brief Do an state or mode phase transition. This will do the transition
     * and call the corresponding callbacks
     *
     * @note Please acquire the mutex before calling this
     *
     * @param new_state
     * @param new_mode_phase
     */
    void transition(std::optional<States> new_state, std::optional<ModePhase> new_mode_phase);

public:
    struct Callbacks {
        // Called only when the state changes
        std::optional<std::function<void(States)>> state_transition;
        // Called only when the mode phase changes
        std::optional<std::function<void(ModePhase)>> mode_phase_transition;
        // Called when either state or mode phase changes
        std::optional<std::function<void(States, ModePhase)>> any_transition;
    } callbacks;

    ConnectorFSM(Callbacks callbacks, logs::LogIntf log, std::string log_prefix = "");

    States get_state();
    ModePhase get_mode_phase();

    void on_car_connected();
    void on_car_disconnected();
    void on_mode_phase_change(ModePhase mode_phase);
    void on_module_placeholder_allocation_response(bool request_successful);
    void on_hmac_key_received();
};

class Dispenser;

class Connector {
    typedef fusion_charger::modbus_driver::raw_registers::WorkingStatus WorkingStatus;

    typedef fusion_charger::modbus_driver::raw_registers::PsuOutputPortAvailability PsuOutputPortAvailability;

    typedef fusion_charger::modbus_driver::raw_registers::ConnectionStatus ConnectionStatus;

    typedef fusion_charger::modbus_driver::ConnectorRegistersConfig ConnectorRegistersConfig;

    typedef fusion_charger::modbus_driver::ConnectorRegisters ConnectorRegisters;

    friend class Dispenser;

public:
    Connector(ConnectorConfig connector_config, std::uint16_t local_connector_number, DispenserConfig dispenser_config,
              logs::LogIntf log);
    Connector(const Connector&) = delete;
    ~Connector();

    // apply from dispenser (partly)
    void start();
    // apply from dispenser (partly)
    void stop();

    /// @brief Check whether the last module placeholder allocation failed.
    /// @return true if the last module placeholder allocation failed.
    bool module_placeholder_allocation_failed();

    PsuOutputPortAvailability get_output_port_availability();

    /// @brief Get the currently published power capabilities. Currently there
    /// might only be the power set.
    /// @return
    Capabilities get_capabilities();

    /// @brief Set the total historical energy charged. This is one value per
    /// dispenser.
    /// @param energy_charged total energy charged in kWh; Precision: 3.; Range:
    /// 0-4_294_967_295
    void set_total_historical_energy_charged_per_connector(double energy_charged);

    WorkingStatus get_working_status();

    /**
     * @brief Please call this if the car gets connected
     *
     */
    void on_car_connected();

    /**
     * @brief Please call this if the car gets disconnected
     *
     */
    void on_car_disconnected();

    /**
     * @brief This should be called if the current charging mode/phase changed
     *
     * @param mode_phase the new mode phase
     */
    void on_mode_phase_change(ModePhase mode_phase);

    /**
     * @brief This should be called if a new export voltage and current is
     * available
     *
     * @param voltage the new export voltage
     * @param current the new export current
     */
    void new_export_voltage_current(double voltage, double current);

    /**
     * @brief Does an car connect - disconnect cycle blockingly, while only trying
     * to acquire the HMAC key
     *
     * @param timeout the timeout to wait for the HMAC key
     */
    void car_connect_disconnect_cycle(std::chrono::milliseconds timeout);

    /**
     * @brief Get the current hmac key for this connector.
     *
     * @return the current hmac key
     */
    std::vector<std::uint8_t> get_hmac_key();

    /**
     * @brief Reset all stored PSU capabilities from our registers
     */
    void reset_psu_capabilities();

private:
    logs::LogIntf log;
    std::string log_prefix;               // Prefix for log messages
    std::uint16_t local_connector_number; // 1-4
    ConnectorConfig connector_config;
    DispenserConfig dispenser_config;
    std::shared_ptr<goose_ethernet::EthernetInterface> eth_interface;
    ConnectorGooseSender goose_sender;
    ConnectorRegistersConfig connector_registers_config;
    ConnectorRegisters connector_registers;

    std::optional<float> rated_output_power_psu; // in kW, set by register callback
    std::optional<float> max_rated_psu_voltage;  // in V, set by register callback
    std::optional<float> max_rated_psu_current;  // in A, set by register callback

    ConnectorFSM fsm;
    bool last_module_placeholder_allocation_failed;

    struct {
        double voltage;
        double current;
    } current_requested_voltage_current;

    struct {
        std::thread thread;
        std::mutex received_mutex;
        std::condition_variable received_cv;
    } module_placeholder_allocation_timeout;

    /**
     * @brief (Re-)send the currently needed goose frame according to the current
     * state, modephase and voltage/current
     *
     * @param state the current state
     * @param mode_phase the current mode phase
     */
    void send_needed_goose_frame(States state, ModePhase mode_phase);

    /**
     * @brief Same as \c send_needed_goose_frame but current state and modephase
     * are taken from the fsm
     */
    void send_needed_goose_frame();

    /**
     * @brief Set the working status modbus register
     *
     * @param status the new working status
     */
    void set_working_status(WorkingStatus status);

    /**
     * @brief Set the connection status modbus register
     *
     * @param status the new connection status
     */
    void set_connection_status(ConnectionStatus status);

    /**
     * @brief Called upon module placeholder allocation response by the dispenser
     *
     * @param request_successful true if the request was successful
     */
    void on_module_placeholder_allocation_response(bool request_successful);

    /**
     * @brief Called when the psu sends a new mac address for goose frames
     *
     * @param mac_address the new mac address
     */
    void on_psu_mac_change(std::vector<std::uint8_t> mac_address);

    /**
     * @brief Called by the FSM when a state transition happens
     *
     * Reports an connector event (if necessary), updates the connector connection
     * status and resets \c last_module_placeholder_allocation_failed if possible
     *
     * @param state the new state
     */
    void on_state_transition(States state);

    /**
     * @brief Called by the FSM when any transition happens. Either the state, the
     * mode phase or both changed
     *
     * Updates the working status and calls \c send_needed_goose_frame
     *
     * @param state the new state
     * @param mode_phase the new mode phase
     */
    void on_state_mode_phase_transition(States state, ModePhase mode_phase);

    /**
     * @brief Called when the state is ConnectedNoAllocation and no response came
     * in time ( \c dispenser_config.module_placeholder_allocation_timeout ).
     *
     */
    void on_module_placeholder_allocation_timeout();

    /**
     * @brief Stop the thread which waits for the module placeholder allocation.
     * This function can be called even when the thread is not running.
     */
    void cancel_module_placeholder_allocation_timeout();
};
