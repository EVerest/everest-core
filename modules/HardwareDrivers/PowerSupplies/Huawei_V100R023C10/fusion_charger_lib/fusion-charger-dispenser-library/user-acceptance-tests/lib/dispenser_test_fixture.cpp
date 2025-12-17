// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "user_acceptance_tests/dispenser_test_fixture.hpp"

#include <gtest/gtest.h>

#include "configuration.hpp"

namespace user_acceptance_tests {
namespace dispenser_fixture {

using namespace std;

const ConnectorCallbacks default_connector_callbacks = ConnectorCallbacks{
    []() { return 0.0f; },
    []() { return 0.0f; },
    []() { return 0.0f; },
    []() { return ContactorStatus::ON; },
    []() { return ElectronicLockStatus::UNLOCKED; },
};

const DispenserConfig dispenser_config_without_tls = DispenserConfig{
    "127.0.0.1",
    8502,
    "veth0",
    0x0002,
    0x0080,
    0x0001,
    0x0003,
    "v1.2.3+456",
    1,
    "01234567890ABCDEF",
    std::chrono::seconds(2),
    true,
    false,
    true,
    nullopt,
    std::chrono::seconds(3),
};

const PowerStackMockConfig default_power_stack_mock_config_without_tls = PowerStackMockConfig{
    "veth1",
    8502,
    {0x67, 0xe4, 0x26, 0x56, 0x0a, 0x70, 0xca, 0x4a, 0x83, 0x3c, 0x44, 0xb3, 0x12, 0x70, 0xca, 0x93,
     0x55, 0xd8, 0x7b, 0x02, 0x0f, 0x57, 0x8e, 0x1e, 0x9d, 0x19, 0x74, 0xc0, 0x2f, 0xa6, 0xf6, 0x80,
     0x4c, 0x2f, 0xcb, 0xdf, 0x73, 0x5e, 0x71, 0x1c, 0xec, 0x08, 0x5b, 0x93, 0x81, 0x47, 0x16, 0xad},
    true,
    true,
    std::nullopt,
};

DispenserConfig dispenser_config_with_tls = DispenserConfig{
    "127.0.0.1",
    8502,
    "veth0",
    0x0002,
    0x0080,
    0x0001,
    0x0003,
    "v1.2.3+456",
    1,
    "01234567890ABCDEF",
    std::chrono::seconds(2),
    true,
    false,
    true,
    tls_util::MutualTlsClientConfig{"modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
                                    "fusion_charger_lib/fusion-charger-dispenser-library/"
                                    "user-acceptance-tests/"
                                    "test_certificates/"
                                    "psu_ca.crt.pem",
                                    "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
                                    "fusion_charger_lib/fusion-charger-dispenser-library/"
                                    "user-acceptance-tests/"
                                    "test_certificates/"
                                    "dispenser.crt.pem",
                                    "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
                                    "fusion_charger_lib/fusion-charger-dispenser-library/"
                                    "user-acceptance-tests/"
                                    "test_certificates/"
                                    "dispenser.key.pem"},
    std::chrono::seconds(3),
};

const PowerStackMockConfig default_power_stack_mock_config_with_tls = PowerStackMockConfig{
    "veth1",
    8502,
    {0x67, 0xe4, 0x26, 0x56, 0x0a, 0x70, 0xca, 0x4a, 0x83, 0x3c, 0x44, 0xb3, 0x12, 0x70, 0xca, 0x93,
     0x55, 0xd8, 0x7b, 0x02, 0x0f, 0x57, 0x8e, 0x1e, 0x9d, 0x19, 0x74, 0xc0, 0x2f, 0xa6, 0xf6, 0x80,
     0x4c, 0x2f, 0xcb, 0xdf, 0x73, 0x5e, 0x71, 0x1c, 0xec, 0x08, 0x5b, 0x93, 0x81, 0x47, 0x16, 0xad},
    true,
    true,
    tls_util::MutualTlsServerConfig{
        "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
        "fusion_charger_lib/fusion-charger-dispenser-library/"
        "user-acceptance-tests/test_certificates/"
        "dispenser_ca.crt.pem",
        "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
        "fusion_charger_lib/fusion-charger-dispenser-library/"
        "user-acceptance-tests/test_certificates/"
        "psu.crt.pem",
        "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
        "fusion_charger_lib/fusion-charger-dispenser-library/"
        "user-acceptance-tests/test_certificates/"
        "psu.key.pem",
    },
};

DispenserTestBase::DispenserTestBase(DispenserTestParams params) :
    dispenser_config(params.dispenser_config),
    connector_configs(params.connector_configs),
    dispenser_connector_upstream_voltage(params.dispenser_connector_upstream_voltage),
    dispenser_output_voltage(params.dispenser_output_voltage),
    dispesner_output_current(params.dispesner_output_current),
    dispenser_contactor_status(params.dispenser_contactor_status),
    dispenser_electronic_lock_status(params.dispenser_electronic_lock_status),
    power_stack_mock_config(params.power_stack_mock_config) {
}

void DispenserTestBase::SetUp() {
    cout << "=-=-=-=-=-= SetUp start =-=-=-=-=-=" << endl;
    dispenser = std::make_shared<Dispenser>(dispenser_config, connector_configs);
    dispenser->start();
    power_stack_mock = std::shared_ptr<PowerStackMock>(PowerStackMock::from_config(power_stack_mock_config));
    power_stack_mock->start_modbus_event_loop();
    sleep_for_ms(20);
    cout << "=-=-=-=-=-= SetUp complete =-=-=-=-=-=" << endl;
}

void DispenserTestBase::TearDown() {
    cout << "=-=-=-=-=-= TearDown started =-=-=-=-=-=" << endl;
    sleep_for_ms(20);
    dispenser->stop();
    power_stack_mock->stop();
    cout << "=-=-=-=-=-= TearDown complete =-=-=-=-=-=" << endl;
}

void DispenserTestBase::sleep_for_ms(std::uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

DispenserWithTlsTest::DispenserWithTlsTest() :
    DispenserTestBase(DispenserTestParams{
        dispenser_config_with_tls, // dispenser_config
        {ConnectorConfig{
            // connector_configs
            5,                   // global_connector_number
            ConnectorType::CCS2, // connector_type
            100.0,               // max_rated_charge_current
            0.0,                 // max_rated_output_power
            ConnectorCallbacks{
                // connector_callbacks
                [this]() { // connector_upstream_voltage
                    return this->dispenser_connector_upstream_voltage.load();
                },
                [this]() { // output_voltage
                    return this->dispenser_output_voltage.load();
                },
                [this]() { // output_current
                    return this->dispesner_output_current.load();
                },
                [this]() { // contactor_status
                    return this->dispenser_contactor_status.load();
                },
                [this]() { // electronic_lock_status
                    return this->dispenser_electronic_lock_status.load();
                },
            },
        }},
        0.0,                                      // dispenser_connector_upstream_voltage
        0.0,                                      // dispenser_output_voltage
        0.0,                                      // dispesner_output_current
        ContactorStatus::OFF,                     // dispenser_contactor_status
        ElectronicLockStatus::UNLOCKED,           // dispenser_electronic_lock_status
        default_power_stack_mock_config_with_tls, // power_stack_mock_config
    }) {
}

std::shared_ptr<Connector> DispenserWithTlsTest::connector() {
    return dispenser->get_connector(local_connector_number);
}

std::optional<fusion_charger::goose::PowerRequirementRequest>
DispenserWithTlsTest::get_last_power_requirement_request() {
    return power_stack_mock->get_last_power_requirement_request(global_connector_number);
}

std::uint32_t DispenserWithTlsTest::get_stop_request_counter() {
    return power_stack_mock->get_stop_charge_request_counter(global_connector_number);
}

std::uint32_t DispenserWithTlsTest::get_power_requirements_counter() {
    return power_stack_mock->get_power_requirements_counter(global_connector_number);
}

float DispenserWithTlsTest::get_maximum_rated_charge_current() {
    return power_stack_mock->get_maximum_rated_charge_current(local_connector_number);
}

ConnectionStatus DispenserWithTlsTest::get_connection_status() {
    return power_stack_mock->get_connection_status(local_connector_number);
}

DispenserWithoutTlsTest::DispenserWithoutTlsTest() :
    DispenserTestBase(DispenserTestParams{
        dispenser_config_without_tls, // dispenser_config
        {ConnectorConfig{
            // connector_configs
            5,                   // global_connector_number
            ConnectorType::CCS2, // connector_type
            100.0,               // max_rated_charge_current
            0.0,                 // max_rated_output_power
            ConnectorCallbacks{
                // connector_callbacks
                [this]() { // connector_upstream_voltage
                    return this->dispenser_connector_upstream_voltage.load();
                },
                [this]() { // output_voltage
                    return this->dispenser_output_voltage.load();
                },
                [this]() { // output_current
                    return this->dispesner_output_current.load();
                },
                [this]() { // contactor_status
                    return this->dispenser_contactor_status.load();
                },
                [this]() { // electronic_lock_status
                    return this->dispenser_electronic_lock_status.load();
                },
            },
        }},
        0.0,                                         // dispenser_connector_upstream_voltage
        0.0,                                         // dispenser_output_voltage
        0.0,                                         // dispesner_output_current
        ContactorStatus::OFF,                        // dispenser_contactor_status
        ElectronicLockStatus::UNLOCKED,              // dispenser_electronic_lock_status
        default_power_stack_mock_config_without_tls, // power_stack_mock_config
    }) {
}

std::shared_ptr<Connector> DispenserWithoutTlsTest::connector() {
    return dispenser->get_connector(local_connector_number);
}

std::optional<fusion_charger::goose::PowerRequirementRequest>
dispenser_fixture::DispenserWithoutTlsTest::get_last_power_requirement_request() {
    return power_stack_mock->get_last_power_requirement_request(global_connector_number);
}

std::uint32_t DispenserWithoutTlsTest::get_stop_request_counter() {
    return power_stack_mock->get_stop_charge_request_counter(global_connector_number);
}

std::uint32_t DispenserWithoutTlsTest::get_power_requirements_counter() {
    return power_stack_mock->get_power_requirements_counter(global_connector_number);
}

float DispenserWithoutTlsTest::get_maximum_rated_charge_current() {
    return power_stack_mock->get_maximum_rated_charge_current(local_connector_number);
}

ConnectionStatus DispenserWithoutTlsTest::get_connection_status() {
    return power_stack_mock->get_connection_status(local_connector_number);
}

dispenser_fixture::DispenserWithMultipleConnectors::DispenserWithMultipleConnectors() :
    DispenserTestBase(DispenserTestParams{
        dispenser_config_with_tls, // dispenser_config
        {
            // connector_configs
            ConnectorConfig{5,                   // global_connector_number
                            ConnectorType::CCS2, // connector_type
                            100.0,               // max_rated_charge_current
                            0.0,                 // max_rated_output_power
                            ConnectorCallbacks{
                                []() { return 100; },                            // connector_upstream_voltage
                                []() { return 101; },                            // output_voltage
                                []() { return 102; },                            // output_current
                                []() { return ContactorStatus::ON; },            // contactor_status
                                []() { return ElectronicLockStatus::UNLOCKED; }, // electronic_lock_status
                            }},
            ConnectorConfig{10,                  // global_connector_number
                            ConnectorType::CCS2, // connector_type
                            200.0,               // max_rated_charge_current
                            0.0,                 // max_rated_output_power
                            ConnectorCallbacks{
                                []() { return 200; },                          // connector_upstream_voltage
                                []() { return 201; },                          // output_voltage
                                []() { return 202; },                          // output_current
                                []() { return ContactorStatus::ON; },          // contactor_status
                                []() { return ElectronicLockStatus::LOCKED; }, // electronic_lock_status
                            }},
            ConnectorConfig{15,                  // global_connector_number
                            ConnectorType::CCS2, // connector_type
                            300.0,               // max_rated_charge_current
                            0.0,                 // max_rated_output_power
                            ConnectorCallbacks{
                                []() { return 300; },                            // connector_upstream_voltage
                                []() { return 301; },                            // output_voltage
                                []() { return 302; },                            // output_current
                                []() { return ContactorStatus::OFF; },           // contactor_status
                                []() { return ElectronicLockStatus::UNLOCKED; }, // electronic_lock_status
                            }},
            ConnectorConfig{4,                   // global_connector_number
                            ConnectorType::CCS2, // connector_type
                            400.0,               // max_rated_charge_current
                            0.0,                 // max_rated_output_power
                            ConnectorCallbacks{
                                []() { return 400; },                          // connector_upstream_voltage
                                []() { return 401; },                          // output_voltage
                                []() { return 402; },                          // output_current
                                []() { return ContactorStatus::OFF; },         // contactor_status
                                []() { return ElectronicLockStatus::LOCKED; }, // electronic_lock_status
                            }},
        },
        0.0,                                      // dispenser_connector_upstream_voltage
        0.0,                                      // dispenser_output_voltage
        0.0,                                      // dispesner_output_current
        ContactorStatus::OFF,                     // dispenser_contactor_status
        ElectronicLockStatus::UNLOCKED,           // dispenser_electronic_lock_status
        default_power_stack_mock_config_with_tls, // power_stack_mock_config
    })

{
}

std::shared_ptr<Connector> DispenserWithMultipleConnectors::get_connector(std::uint16_t local_connector_number) {
    return dispenser->get_connector(local_connector_number);
}

void DispenserWithMultipleConnectors::set_up_psu_for_operation() {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();
    sleep_for_ms(10);
}

void DispenserWithMultipleConnectors::connect_car(std::uint16_t local_connector_number) {
    get_connector(local_connector_number)->on_car_connected();
    sleep_for_ms(10);
}
void DispenserWithMultipleConnectors::send_hmac_key(std::uint16_t local_connector_number) {
    power_stack_mock->send_hmac_key(local_connector_number);
    sleep_for_ms(10);
}

void DispenserWithMultipleConnectors::set_export_values(std::uint16_t local_connector_number, float voltage,
                                                        float current) {
    get_connector(local_connector_number)->new_export_voltage_current(voltage, current);
    sleep_for_ms(10);
}

void DispenserWithMultipleConnectors::set_mode_phase(std::uint16_t local_connector_number, ModePhase mode_phase) {
    get_connector(local_connector_number)->on_mode_phase_change(mode_phase);
    sleep_for_ms(10);
}

std::array<std::uint32_t, 4> DispenserWithMultipleConnectors::get_stop_request_counter() {
    auto result = std::array<std::uint32_t, 4>();
    for (int i = 0; i < 4; i++) {
        auto counter = power_stack_mock->get_stop_charge_request_counter(connector_configs[i].global_connector_number);

        result[i] = counter;
    }

    return result;
}

void DispenserWithMultipleConnectors::disconnect_car(std::uint16_t local_connector_number) {
    get_connector(local_connector_number)->on_car_disconnected();
    sleep_for_ms(10);
}

void DispenserWithMultipleConnectors::assert_working_status(std::array<WorkingStatus, 4> expected_status) {
    for (int i = 0; i < 4; i++) {
        auto actual_status = get_connector(i + 1)->get_working_status();
        EXPECT_EQ(actual_status, expected_status[i]) << "Regarding connector " << i + 1;
    }
}

void DispenserWithMultipleConnectors::assert_requirement_type(
    std::array<std::optional<fusion_charger::goose::RequirementType>, 4> expected_types) {
    for (int i = 0; i < 4; i++) {
        auto actual_status =
            power_stack_mock->get_last_power_requirement_request(connector_configs[i].global_connector_number);

        if (actual_status == nullopt && expected_types[i] == nullopt) {
            continue;
        } else if (actual_status == nullopt) {
            FAIL() << "Actual Status is NULL , but expected was: " << (std::uint16_t)expected_types[i].value()
                   << " regarding connector " << i + 1;
        } else if (expected_types[i] == nullopt) {
            FAIL() << "Actual Status is: " << (std::uint16_t)actual_status.value().requirement_type
                   << " , but expected was NULL"
                   << " regarding connector " << i + 1;
        }

        EXPECT_EQ(actual_status->requirement_type, expected_types[i]) << "Regarding connector " << i + 1;
    }
}

void DispenserWithMultipleConnectors::assert_stop_request_counter_greater_or_equal(
    std::array<std::uint32_t, 4> expected) {
    for (int i = 0; i < 4; i++) {
        auto actual = power_stack_mock->get_stop_charge_request_counter(connector_configs[i].global_connector_number);

        EXPECT_GE(actual, expected[i]) << "Regarding connector " << i + 1;
    }
}

} // namespace dispenser_fixture

} // namespace user_acceptance_tests
