// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include "raw.hpp"
#include "utils.hpp"

namespace fusion_charger::modbus_driver {
using namespace modbus::registers::data_providers;
using namespace modbus_extensions;

typedef raw_registers::PsuOutputPortAvailability PsuOutputPortAvailability;

using ConnectorOffset = raw_registers::ConnectorOffset;

struct ConnectorRegistersConfig {
    using ConnectorType = raw_registers::ConnectorType;
    using ContactorStatus = raw_registers::CollectedConnectorRegisters::ContactorStatus;
    using ElectronicLockStatus = raw_registers::CollectedConnectorRegisters::ElectronicLockStatus;

    std::uint8_t mac_address[6];
    ConnectorType type;
    std::uint16_t global_connector_no;
    std::uint16_t connector_number;
    float max_rated_charge_current;
    float rated_output_power_connector;
    std::function<float()> get_contactor_upstream_voltage;
    std::function<float()> get_output_voltage;
    std::function<float()> get_output_current;
    std::function<ContactorStatus()> get_contactor_status;
    std::function<ElectronicLockStatus()> get_electronic_lock_status;
    std::function<bool()> get_dc_output_contact_fault;
};

struct ConnectorRegisters {
    using ConnectorType = raw_registers::ConnectorType;
    using WorkingStatus = raw_registers::WorkingStatus;
    using ConnectionStatus = raw_registers::ConnectionStatus;

    using ContactorStatus = raw_registers::CollectedConnectorRegisters::ContactorStatus;
    using ElectronicLockStatus = raw_registers::CollectedConnectorRegisters::ElectronicLockStatus;
    using ChargingEventConnector = raw_registers::CollectedConnectorRegisters::ChargingEventConnector;

    /// @brief Connector number on dispenser (1-4)
    std::uint16_t connector_number;

    DataProviderHolding<double> total_energy_charged;
    DataProviderHolding<ConnectorType> connector_type;
    // Reg 0x1105
    DataProviderHolding<float> maximum_rated_charge_current;
    DataProviderCallbacks<float> output_voltage;
    DataProviderCallbacks<float> output_current;
    DataProviderHoldingUnsolicitatedReportCallback<WorkingStatus> working_status;
    DataProviderHoldingUnsolicitatedReportCallback<ConnectionStatus> connection_status;
    DataProviderHolding<std::uint16_t> connector_no; // 1-12
    DataProviderCallbacks<float> contactor_upstream_voltage;
    DataProviderMemoryHolding<6> mac_address;
    // "Status of contactors (DC+, DC-)" 0 off, 1 on
    DataProviderCallbacksUnsolicitated<ContactorStatus> contactor_status;
    // unlocked = 0, locked = 1
    DataProviderCallbacksUnsolicitated<ElectronicLockStatus> electronic_lock_status;
    DataProviderUnsolicitatedEvent<ChargingEventConnector> charging_event_connector; // todo ??

    // from dispenser
    DataProviderHolding<float> max_rated_psu_voltage;
    DataProviderHolding<float> max_rated_psu_current;
    DataProviderHolding<float> min_rated_psu_voltage;
    DataProviderHolding<float> min_rated_psu_current;

    DataProviderHolding<float> rated_output_power_connector;
    DataProviderMemoryHolding<48> hmac_key;
    DataProviderHolding<float> rated_output_power_psu;
    // written by dispenser
    DataProviderHolding<PsuOutputPortAvailability> psu_port_available;

    // alarms
    DataProviderCallbacksUnsolicitated<std::uint16_t> dc_output_contact_fault;
    DataProviderHoldingUnsolicitatedReportCallback<std::uint16_t> inverse_connection_dispenser_inlet_cable;

    /**
     * @param connector connecter number in dispenser 1-4
     * @param global_connector_no connector number in the whole system 1-12
     * @param type Connector type \c ConnectorType
     * @param max_charge_current 0x1105: The maximum rated charging current of the
     * connector
     * @param get_output_voltage callback to get current output voltage (seems to
     * be voltage near car)
     * @param get_output_current callback to get current output current
     * @param get_contactor_upstream_voltage callback to get upstream voltage
     * (seems to be voltage near charger)
     * @param mac_address MAC address of the dispenser
     * @param rated_output_power_connector
     */
    ConnectorRegisters(ConnectorRegistersConfig config) :
        connector_number(config.connector_number),
        total_energy_charged(0),
        connector_type(config.type),
        maximum_rated_charge_current(config.max_rated_charge_current),
        output_voltage(config.get_output_voltage, utils::ignore_write<float>),
        output_current(config.get_output_current, utils::ignore_write<float>),
        working_status(WorkingStatus::STANDBY, utils::always_report),
        connection_status(ConnectionStatus::NOT_CONNECTED, utils::always_report),
        connector_no(config.global_connector_no),
        contactor_upstream_voltage(config.get_contactor_upstream_voltage, utils::ignore_write<float>),
        mac_address(config.mac_address),
        contactor_status(config.get_contactor_status, utils::ignore_write<ContactorStatus>, utils::always_report),
        electronic_lock_status(config.get_electronic_lock_status, utils::ignore_write<ElectronicLockStatus>,
                               utils::always_report),
        charging_event_connector(ChargingEventConnector::START_TO_STOP),
        max_rated_psu_voltage(0),
        max_rated_psu_current(0),
        min_rated_psu_voltage(0),
        min_rated_psu_current(0),
        rated_output_power_connector(config.rated_output_power_connector),
        hmac_key(),
        rated_output_power_psu(0),
        psu_port_available(PsuOutputPortAvailability::NOT_AVAILABLE),
        dc_output_contact_fault(utils::wrap_alarm_register_func(config.get_dc_output_contact_fault),
                                utils::ignore_write<std::uint16_t>, utils::always_report),
        inverse_connection_dispenser_inlet_cable(0, utils::always_report) {
    }

    void add_to_registry(modbus::registers::registry::ComplexRegisterRegistry& registry) {
        ConnectorOffset offset =
            fusion_charger::modbus_driver::raw_registers::offset_from_connector_number(connector_number);

        raw_registers::CollectedConnectorRegisters::DataProviders collected_connector_registers{
            total_energy_charged,
            connector_type,
            maximum_rated_charge_current,
            output_voltage,
            output_current,
            working_status,
            connection_status,
            connector_no,
            contactor_upstream_voltage,
            mac_address,
            contactor_status,
            electronic_lock_status,
            charging_event_connector,
        };

        raw_registers::SettingConnectorRegisters::DataProviders setting_connector_registers{
            max_rated_psu_voltage,  max_rated_psu_current,        min_rated_psu_voltage,
            min_rated_psu_current,  rated_output_power_connector, hmac_key,
            rated_output_power_psu, psu_port_available,
        };

        raw_registers::AlarmConnectorRegisters::DataProviders alarm_connector_registers{
            dc_output_contact_fault, inverse_connection_dispenser_inlet_cable};

        registry.add(
            std::make_unique<raw_registers::CollectedConnectorRegisters>(offset, collected_connector_registers));
        registry.add(std::make_unique<raw_registers::SettingConnectorRegisters>(offset, setting_connector_registers));
        registry.add(std::make_unique<raw_registers::AlarmConnectorRegisters>(offset, alarm_connector_registers));
    }
};

} // namespace fusion_charger::modbus_driver
