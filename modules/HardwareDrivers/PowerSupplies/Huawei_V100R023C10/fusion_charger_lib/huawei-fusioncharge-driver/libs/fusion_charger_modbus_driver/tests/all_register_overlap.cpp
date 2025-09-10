// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <fusion_charger/modbus/registers/connector.hpp>
#include <fusion_charger/modbus/registers/dispenser.hpp>
#include <fusion_charger/modbus/registers/power_unit.hpp>

using namespace fusion_charger::modbus_driver;

TEST(RegisterOverlap, all_connectors_no_overlap) {
    uint8_t mac[6] = {0, 1, 2, 3, 4, 5};

    PowerUnitRegisters power_unit_registers;
    DispenserRegisters dispenser_registers(DispenserRegistersConfig{.esn = "12345678"});

    ConnectorRegistersConfig connector_register_config1 = ConnectorRegistersConfig{
        .mac_address = {*(&mac[0])},
        .type = ConnectorRegisters::ConnectorType::CCS1,
        .global_connector_no = 1,
        .connector_number = 1,
        .max_rated_charge_current = 0.0,
        .rated_output_power_connector = 0.0,
        .get_contactor_upstream_voltage = []() { return 0.0; },
        .get_output_voltage = []() { return 0.0; },
        .get_output_current = []() { return 0.0; },
    };
    ConnectorRegisters connector_registers1(connector_register_config1);

    ConnectorRegistersConfig connector_register_config2 = ConnectorRegistersConfig{
        .mac_address = {*(&mac[0])},
        .type = ConnectorRegisters::ConnectorType::CCS1,
        .global_connector_no = 2,
        .connector_number = 2,
        .max_rated_charge_current = 0.0,
        .rated_output_power_connector = 0.0,
        .get_contactor_upstream_voltage = []() { return 0.0; },
        .get_output_voltage = []() { return 0.0; },
        .get_output_current = []() { return 0.0; },
    };
    ConnectorRegisters connector_registers2(connector_register_config2);

    ConnectorRegistersConfig connector_register_config3 = ConnectorRegistersConfig{
        .mac_address = {*(&mac[0])},
        .type = ConnectorRegisters::ConnectorType::CCS1,
        .global_connector_no = 3,
        .connector_number = 3,
        .max_rated_charge_current = 0.0,
        .rated_output_power_connector = 0.0,
        .get_contactor_upstream_voltage = []() { return 0.0; },
        .get_output_voltage = []() { return 0.0; },
        .get_output_current = []() { return 0.0; },
    };
    ConnectorRegisters connector_registers3(connector_register_config3);

    ConnectorRegistersConfig connector_register_config4 = ConnectorRegistersConfig{
        .mac_address = {*(&mac[0])},
        .type = ConnectorRegisters::ConnectorType::CCS1,
        .global_connector_no = 4,
        .connector_number = 4,
        .max_rated_charge_current = 0.0,
        .rated_output_power_connector = 0.0,
        .get_contactor_upstream_voltage = []() { return 0.0; },
        .get_output_voltage = []() { return 0.0; },
        .get_output_current = []() { return 0.0; },
    };
    ConnectorRegisters connector_registers4(connector_register_config4);

    modbus::registers::registry::ComplexRegisterRegistry registry;
    power_unit_registers.add_to_registry(registry);
    dispenser_registers.add_to_registry(registry);
    connector_registers1.add_to_registry(registry);
    connector_registers2.add_to_registry(registry);
    connector_registers3.add_to_registry(registry);
    connector_registers4.add_to_registry(registry);

    ASSERT_NO_THROW(registry.verify_overlap());
}
