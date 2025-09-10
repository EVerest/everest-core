// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <fusion_charger/modbus/registers/raw.hpp>

using namespace fusion_charger::modbus_driver::raw_registers;

TEST(CommonDispenserRegisters, no_internal_overlap) {
    DataProviderHolding<uint16_t> manufacturer(0);
    DataProviderHolding<uint16_t> model(1);
    DataProviderHolding<uint16_t> protocol_version(2);
    DataProviderHolding<uint16_t> hardware_version(2);
    DataProviderStringHolding<48> software_version;

    CommonDispenserRegisters::DataProviders data_providers{manufacturer, model, protocol_version, hardware_version,
                                                           software_version};

    CommonDispenserRegisters dispenser_registers(data_providers);
    ASSERT_NO_THROW(dispenser_registers.verify_internal_overlap());
}

TEST(CommonPowerUnitRegisters, no_internal_overlap) {
    DataProviderHolding<uint16_t> manufacturer(0);
    DataProviderHolding<uint16_t> protocol_version(1);
    DataProviderHolding<uint16_t> hardware_version(2);
    DataProviderStringHolding<48> software_version;
    DataProviderStringHolding<32> esn_control_board;

    CommonPowerUnitRegisters::DataProviders data_providers{manufacturer, protocol_version, hardware_version,
                                                           software_version, esn_control_board};

    CommonPowerUnitRegisters power_unit_registers(data_providers);
    ASSERT_NO_THROW(power_unit_registers.verify_internal_overlap());
}

TEST(CollectedDispenserRegisters, no_internal_overlap) {
    DataProviderHolding<uint16_t> number_of_charging_connectors(0);
    DataProviderStringHolding<22> esn_dispenser;
    DataProviderHoldingUnsolicitatedReportCallback<uint32_t> time_sync(0, []() { return true; });

    CollectedDispenserRegisters::DataProviders data_providers{number_of_charging_connectors, esn_dispenser, time_sync};

    CollectedDispenserRegisters dispenser_registers(data_providers);
    ASSERT_NO_THROW(dispenser_registers.verify_internal_overlap());
}

bool always_true() {
    return true;
}

TEST(CollectedConnectorRegisters, no_internal_overlap) {
    DataProviderHolding<double> total_energy_charged(0);
    DataProviderHolding<ConnectorType> connector_type(ConnectorType::CCS1);
    DataProviderHolding<float> maximum_rated_charge_current(0.0);
    DataProviderHolding<float> output_voltage(0.0);
    DataProviderHolding<float> output_current(0.0);
    DataProviderHoldingUnsolicitatedReportCallback<WorkingStatus> working_status(WorkingStatus::STANDBY, always_true);
    DataProviderHoldingUnsolicitatedReportCallback<ConnectionStatus> connection_status(ConnectionStatus::NOT_CONNECTED,
                                                                                       always_true);
    DataProviderHolding<uint16_t> connector_no(0); // 1-12
    DataProviderHolding<float> contactor_upstream_voltage(0);
    DataProviderMemoryHolding<6> mac_address;
    // 0 off, 1 on
    DataProviderHoldingUnsolicitatedReportCallback<CollectedConnectorRegisters::ContactorStatus> contactor_status(
        CollectedConnectorRegisters::ContactorStatus::OFF, always_true);
    DataProviderHoldingUnsolicitatedReportCallback<CollectedConnectorRegisters::ElectronicLockStatus>
        electronic_lock_status(CollectedConnectorRegisters::ElectronicLockStatus::UNLOCKED, always_true);
    DataProviderHoldingUnsolicitatedReportCallback<CollectedConnectorRegisters::ChargingEventConnector>
        charging_event_connector(CollectedConnectorRegisters::ChargingEventConnector::START_TO_STOP, always_true);

    CollectedConnectorRegisters::DataProviders data_providers{total_energy_charged,
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
                                                              charging_event_connector};

    CollectedConnectorRegisters registers(ConnectorOffset::CONNECTOR_1_OFFSET, data_providers);

    ASSERT_NO_THROW(registers.verify_internal_overlap());
}

TEST(SettingPowerUnitRegisters, no_internal_overlap) {
    DataProviderHolding<SettingPowerUnitRegisters::PSURunningMode> psu_running_mode(
        SettingPowerUnitRegisters::PSURunningMode::FAULTY);
    DataProviderMemoryHolding<6> psu_mac;
    DataProviderHolding<float> ac_input_voltage_a(0);
    DataProviderHolding<float> ac_input_voltage_b(0);
    DataProviderHolding<float> ac_input_voltage_c(0);
    DataProviderHolding<float> ac_input_current_a(0);
    DataProviderHolding<float> ac_input_current_b(0);
    DataProviderHolding<float> ac_input_current_c(0);
    DataProviderHolding<double> total_historic_input_energy(0);

    SettingPowerUnitRegisters::DataProviders data_providers{psu_running_mode,           psu_mac,
                                                            ac_input_voltage_a,         ac_input_voltage_b,
                                                            ac_input_voltage_c,         ac_input_current_a,
                                                            ac_input_current_b,         ac_input_current_c,
                                                            total_historic_input_energy};

    SettingPowerUnitRegisters registers(data_providers);
    ASSERT_NO_THROW(registers.verify_internal_overlap());
}

TEST(SettingConnectorRegisters, no_internal_overlap) {
    DataProviderHolding<float> max_rated_psu_voltage(0);
    DataProviderHolding<float> max_rated_psu_current(0);
    DataProviderHolding<float> min_rated_psu_voltage(0);
    DataProviderHolding<float> min_rated_psu_current(0);
    DataProviderHolding<float> rated_output_power_connector(0);
    DataProviderMemoryHolding<48> hmac_key;
    DataProviderHolding<float> rated_output_power_psu(0);
    DataProviderHolding<PsuOutputPortAvailability> psu_port_available(PsuOutputPortAvailability::NOT_AVAILABLE);

    SettingConnectorRegisters::DataProviders data_providers{max_rated_psu_voltage,        max_rated_psu_current,
                                                            min_rated_psu_voltage,        min_rated_psu_current,
                                                            rated_output_power_connector, hmac_key,
                                                            rated_output_power_psu,       psu_port_available};

    SettingConnectorRegisters registers(ConnectorOffset::CONNECTOR_1_OFFSET, data_providers);
    ASSERT_NO_THROW(registers.verify_internal_overlap());
}
