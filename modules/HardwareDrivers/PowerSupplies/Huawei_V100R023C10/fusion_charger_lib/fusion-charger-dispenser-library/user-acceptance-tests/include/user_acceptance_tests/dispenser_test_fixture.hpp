// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <gtest/gtest.h>

#include "dispenser.hpp"
#include "power_stack_mock/power_stack_mock.hpp"

namespace user_acceptance_tests {
namespace dispenser_fixture {

class DispenserTestBase : public ::testing::Test {
protected:
    struct DispenserTestParams {
        DispenserConfig dispenser_config; // Move this above 'dispenser'
        std::vector<ConnectorConfig> connector_configs;

        float dispenser_connector_upstream_voltage;
        float dispenser_output_voltage;
        float dispesner_output_current;
        ContactorStatus dispenser_contactor_status;
        ElectronicLockStatus dispenser_electronic_lock_status;

        PowerStackMockConfig power_stack_mock_config;
    };

    DispenserConfig dispenser_config; // Move this above 'dispenser'
    std::vector<ConnectorConfig> connector_configs;

    std::atomic<float> dispenser_connector_upstream_voltage;
    std::atomic<float> dispenser_output_voltage;
    std::atomic<float> dispesner_output_current;
    std::atomic<ContactorStatus> dispenser_contactor_status;
    std::atomic<ElectronicLockStatus> dispenser_electronic_lock_status;

    ConnectorCallbacks connector_callbacks;
    std::shared_ptr<Dispenser> dispenser; // Move this below 'dispenser_config'
                                          //
    PowerStackMockConfig power_stack_mock_config;
    std::shared_ptr<PowerStackMock> power_stack_mock;

protected:
    DispenserTestBase(DispenserTestParams params);

    virtual void SetUp() override;
    virtual void TearDown() override;

    virtual void sleep_for_ms(std::uint32_t ms);
};

class DispenserWithTlsTest : public DispenserTestBase {
public:
    DispenserWithTlsTest();

    const std::uint16_t global_connector_number = connector_configs[0].global_connector_number;
    const std::uint16_t local_connector_number = 1;

    std::shared_ptr<Connector> connector();
    std::optional<fusion_charger::goose::PowerRequirementRequest> get_last_power_requirement_request();
    std::uint32_t get_stop_request_counter();
    std::uint32_t get_power_requirements_counter();
    float get_maximum_rated_charge_current();
    ConnectionStatus get_connection_status();
};

class DispenserWithoutTlsTest : public DispenserTestBase {
public:
    DispenserWithoutTlsTest();

    const std::uint16_t global_connector_number = connector_configs[0].global_connector_number;
    const std::uint16_t local_connector_number = 1;

    std::shared_ptr<Connector> connector();
    std::optional<fusion_charger::goose::PowerRequirementRequest> get_last_power_requirement_request();
    std::uint32_t get_stop_request_counter();
    std::uint32_t get_power_requirements_counter();
    float get_maximum_rated_charge_current();
    ConnectionStatus get_connection_status();
};

class DispenserWithMultipleConnectors : public DispenserTestBase {
public:
    std::uint16_t local_connector_number1 = 1;
    std::uint16_t local_connector_number2 = 2;
    std::uint16_t local_connector_number3 = 3;
    std::uint16_t local_connector_number4 = 4;

    DispenserWithMultipleConnectors();

    std::shared_ptr<Connector> get_connector(std::uint16_t local_connector_number);
    void set_up_psu_for_operation();
    void connect_car(std::uint16_t local_connector_number);
    void send_hmac_key(std::uint16_t local_connector_number);
    void set_export_values(std::uint16_t local_connector_number, float voltage, float current);
    void set_mode_phase(std::uint16_t local_connector_number, ModePhase mode_phase);
    std::array<std::uint32_t, 4> get_stop_request_counter();
    void disconnect_car(std::uint16_t local_connector_number);

    void assert_working_status(std::array<WorkingStatus, 4> expected_status);
    void assert_requirement_type(std::array<std::optional<fusion_charger::goose::RequirementType>, 4> expected_types);

    void assert_stop_request_counter_greater_or_equal(std::array<std::uint32_t, 4> expected);
};

} // namespace dispenser_fixture

} // namespace user_acceptance_tests
