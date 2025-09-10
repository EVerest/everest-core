// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "user_acceptance_tests/dispenser_test_fixture.hpp"

#include <gtest/gtest.h>

#include "configuration.hpp"

namespace user_acceptance_tests {
namespace dispenser_fixture {

using namespace std;

const ConnectorCallbacks default_connector_callbacks = ConnectorCallbacks{
    .connector_upstream_voltage = []() { return 0.0f; },
    .output_voltage = []() { return 0.0f; },
    .output_current = []() { return 0.0f; },
    .contactor_status = []() { return ContactorStatus::ON; },
    .electronic_lock_status = []() { return ElectronicLockStatus::UNLOCKED; },
};

const DispenserConfig dispenser_config_without_tls = DispenserConfig{
    .psu_host = "127.0.0.1",
    .psu_port = 8502,
    .eth_interface = "veth0",
    .manufacturer = 0x0002,
    .model = 0x0080,
    .protocol_version = 0x0001,
    .hardware_version = 0x0003,
    .software_version = "v1.2.3+456",
    .charging_connector_count = 1,
    .esn = "01234567890ABCDEF",
    .modbus_timeout_ms = std::chrono::seconds(2),
    .secure_goose = true,
    .tls_config = nullopt,
    .module_placeholder_allocation_timeout = std::chrono::seconds(3),
};

const PowerStackMockConfig default_power_stack_mock_config_without_tls = PowerStackMockConfig{
    .eth = "veth1",
    .port = 8502,
    .hmac_key = {0x67e4, 0x2656, 0x0a70, 0xca4a, 0x833c, 0x44b3, 0x1270, 0xca93, 0x55d8, 0x7b02, 0x0f57, 0x8e1e,
                 0x9d19, 0x74c0, 0x2fa6, 0xf680, 0x4c2f, 0xcbdf, 0x735e, 0x711c, 0xec08, 0x5b93, 0x8147, 0x16ad},
    .tls_config = nullopt,
};

DispenserConfig dispenser_config_with_tls = DispenserConfig{
    .psu_host = "127.0.0.1",
    .psu_port = 8502,
    .eth_interface = "veth0",
    .manufacturer = 0x0002,
    .model = 0x0080,
    .protocol_version = 0x0001,
    .hardware_version = 0x0003,
    .software_version = "v1.2.3+456",
    .charging_connector_count = 1,
    .esn = "01234567890ABCDEF",
    .modbus_timeout_ms = std::chrono::seconds(2),
    .secure_goose = true,
    .tls_config =
        tls_util::MutualTlsClientConfig{.ca_cert = "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
                                                   "fusion_charger_lib/fusion-charger-dispenser-library/"
                                                   "user-acceptance-tests/"
                                                   "test_certificates/"
                                                   "psu_ca.crt.pem",
                                        .client_cert = "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
                                                       "fusion_charger_lib/fusion-charger-dispenser-library/"
                                                       "user-acceptance-tests/"
                                                       "test_certificates/"
                                                       "dispenser.crt.pem",
                                        .client_key = "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
                                                      "fusion_charger_lib/fusion-charger-dispenser-library/"
                                                      "user-acceptance-tests/"
                                                      "test_certificates/"
                                                      "dispenser.key.pem"},
    .module_placeholder_allocation_timeout = std::chrono::seconds(3),
};

const PowerStackMockConfig default_power_stack_mock_config_with_tls = PowerStackMockConfig{
    .eth = "veth1",
    .port = 8502,
    .hmac_key = {0x67e4, 0x2656, 0x0a70, 0xca4a, 0x833c, 0x44b3, 0x1270, 0xca93, 0x55d8, 0x7b02, 0x0f57, 0x8e1e,
                 0x9d19, 0x74c0, 0x2fa6, 0xf680, 0x4c2f, 0xcbdf, 0x735e, 0x711c, 0xec08, 0x5b93, 0x8147, 0x16ad},
    .tls_config =
        tls_util::MutualTlsServerConfig{
            .client_ca = "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
                         "fusion_charger_lib/fusion-charger-dispenser-library/"
                         "user-acceptance-tests/test_certificates/"
                         "dispenser_ca.crt.pem",
            .server_cert = "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
                           "fusion_charger_lib/fusion-charger-dispenser-library/"
                           "user-acceptance-tests/test_certificates/"
                           "psu.crt.pem",
            .server_key = "modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/"
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

void DispenserTestBase::sleep_for_ms(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

DispenserWithTlsTest::DispenserWithTlsTest() :
    DispenserTestBase(DispenserTestParams{
        .dispenser_config = dispenser_config_with_tls,
        .connector_configs = {ConnectorConfig{
            .global_connector_number = 5,
            .connector_type = ConnectorType::CCS2,
            .max_rated_charge_current = 100.0,
            .connector_callbacks = ConnectorCallbacks{
                .connector_upstream_voltage = [this]() { return this->dispenser_connector_upstream_voltage.load(); },
                .output_voltage = [this]() { return this->dispenser_output_voltage.load(); },
                .output_current = [this]() { return this->dispesner_output_current.load(); },
                .contactor_status = [this]() { return this->dispenser_contactor_status.load(); },
                .electronic_lock_status = [this]() { return this->dispenser_electronic_lock_status.load(); },
            },
        }},
        .dispenser_connector_upstream_voltage = 0.0,
        .dispenser_output_voltage = 0.0,
        .dispesner_output_current = 0.0,
        .dispenser_contactor_status = ContactorStatus::OFF,
        .dispenser_electronic_lock_status = ElectronicLockStatus::UNLOCKED,
        .power_stack_mock_config = default_power_stack_mock_config_with_tls,
    }) {
}

std::shared_ptr<Connector> DispenserWithTlsTest::connector() {
    return dispenser->get_connector(local_connector_number);
}

std::optional<fusion_charger::goose::PowerRequirementRequest>
DispenserWithTlsTest::get_last_power_requirement_request() {
    return power_stack_mock->get_last_power_requirement_request(global_connector_number);
}

uint32_t DispenserWithTlsTest::get_stop_request_counter() {
    return power_stack_mock->get_stop_charge_request_counter(global_connector_number);
}

uint32_t DispenserWithTlsTest::get_power_requirements_counter() {
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
        .dispenser_config = dispenser_config_without_tls,
        .connector_configs = {ConnectorConfig{
            .global_connector_number = 5,
            .connector_type = ConnectorType::CCS2,
            .max_rated_charge_current = 100.0,
            .connector_callbacks = ConnectorCallbacks{
                .connector_upstream_voltage = [this]() { return this->dispenser_connector_upstream_voltage.load(); },
                .output_voltage = [this]() { return this->dispenser_output_voltage.load(); },
                .output_current = [this]() { return this->dispesner_output_current.load(); },
                .contactor_status = [this]() { return this->dispenser_contactor_status.load(); },
                .electronic_lock_status = [this]() { return this->dispenser_electronic_lock_status.load(); },
            },
        }},
        .dispenser_connector_upstream_voltage = 0.0,
        .dispenser_output_voltage = 0.0,
        .dispesner_output_current = 0.0,
        .dispenser_contactor_status = ContactorStatus::OFF,
        .dispenser_electronic_lock_status = ElectronicLockStatus::UNLOCKED,
        .power_stack_mock_config = default_power_stack_mock_config_without_tls,
    }) {
}

std::shared_ptr<Connector> DispenserWithoutTlsTest::connector() {
    return dispenser->get_connector(local_connector_number);
}

std::optional<fusion_charger::goose::PowerRequirementRequest>
dispenser_fixture::DispenserWithoutTlsTest::get_last_power_requirement_request() {
    return power_stack_mock->get_last_power_requirement_request(global_connector_number);
}

uint32_t DispenserWithoutTlsTest::get_stop_request_counter() {
    return power_stack_mock->get_stop_charge_request_counter(global_connector_number);
}

uint32_t DispenserWithoutTlsTest::get_power_requirements_counter() {
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
        .dispenser_config = dispenser_config_with_tls,
        .connector_configs =
            {
                ConnectorConfig{.global_connector_number = 5,
                                .connector_type = ConnectorType::CCS2,
                                .max_rated_charge_current = 100.0,
                                .connector_callbacks = ConnectorCallbacks{
                                    .connector_upstream_voltage = []() { return 100; },
                                    .output_voltage = []() { return 101; },
                                    .output_current = []() { return 102; },
                                    .contactor_status = []() { return ContactorStatus::ON; },
                                    .electronic_lock_status = []() { return ElectronicLockStatus::UNLOCKED; },

                                }},
                ConnectorConfig{.global_connector_number = 10,
                                .connector_type = ConnectorType::CCS2,
                                .max_rated_charge_current = 200.0,
                                .connector_callbacks = ConnectorCallbacks{
                                    .connector_upstream_voltage = []() { return 200; },
                                    .output_voltage = []() { return 201; },
                                    .output_current = []() { return 202; },
                                    .contactor_status = []() { return ContactorStatus::ON; },
                                    .electronic_lock_status = []() { return ElectronicLockStatus::LOCKED; },

                                }},

                ConnectorConfig{.global_connector_number = 15,
                                .connector_type = ConnectorType::CCS2,
                                .max_rated_charge_current = 300.0,
                                .connector_callbacks = ConnectorCallbacks{
                                    .connector_upstream_voltage = []() { return 300; },
                                    .output_voltage = []() { return 301; },
                                    .output_current = []() { return 302; },
                                    .contactor_status = []() { return ContactorStatus::OFF; },
                                    .electronic_lock_status = []() { return ElectronicLockStatus::UNLOCKED; },

                                }},
                ConnectorConfig{.global_connector_number = 4,
                                .connector_type = ConnectorType::CCS2,
                                .max_rated_charge_current = 400.0,
                                .connector_callbacks = ConnectorCallbacks{
                                    .connector_upstream_voltage = []() { return 400; },
                                    .output_voltage = []() { return 401; },
                                    .output_current = []() { return 402; },
                                    .contactor_status = []() { return ContactorStatus::OFF; },
                                    .electronic_lock_status = []() { return ElectronicLockStatus::LOCKED; },

                                }},

            },
        .dispenser_connector_upstream_voltage = 0.0,
        .dispenser_output_voltage = 0.0,
        .dispesner_output_current = 0.0,
        .dispenser_contactor_status = ContactorStatus::OFF,
        .dispenser_electronic_lock_status = ElectronicLockStatus::UNLOCKED,
        .power_stack_mock_config = default_power_stack_mock_config_with_tls,
    })

{
}

std::shared_ptr<Connector> DispenserWithMultipleConnectors::get_connector(uint16_t local_connector_number) {
    return dispenser->get_connector(local_connector_number);
}

void DispenserWithMultipleConnectors::set_up_psu_for_operation() {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();
    sleep_for_ms(10);
}

void DispenserWithMultipleConnectors::connect_car(uint16_t local_connector_number) {
    get_connector(local_connector_number)->on_car_connected();
    sleep_for_ms(10);
}
void DispenserWithMultipleConnectors::send_hmac_key(uint16_t local_connector_number) {
    power_stack_mock->send_hmac_key(local_connector_number);
    sleep_for_ms(10);
}

void DispenserWithMultipleConnectors::set_export_values(uint16_t local_connector_number, float voltage, float current) {
    get_connector(local_connector_number)->new_export_voltage_current(voltage, current);
    sleep_for_ms(10);
}

void DispenserWithMultipleConnectors::set_mode_phase(uint16_t local_connector_number, ModePhase mode_phase) {
    get_connector(local_connector_number)->on_mode_phase_change(mode_phase);
    sleep_for_ms(10);
}

std::array<uint32_t, 4> DispenserWithMultipleConnectors::get_stop_request_counter() {
    auto result = std::array<uint32_t, 4>();
    for (int i = 0; i < 4; i++) {
        auto counter = power_stack_mock->get_stop_charge_request_counter(connector_configs[i].global_connector_number);

        result[i] = counter;
    }

    return result;
}

void DispenserWithMultipleConnectors::disconnect_car(uint16_t local_connector_number) {
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
            FAIL() << "Actual Status is NULL , but expected was: " << (uint16_t)expected_types[i].value()
                   << " regarding connector " << i + 1;
        } else if (expected_types[i] == nullopt) {
            FAIL() << "Actual Status is: " << (uint16_t)actual_status.value().requirement_type
                   << " , but expected was NULL"
                   << " regarding connector " << i + 1;
        }

        EXPECT_EQ(actual_status->requirement_type, expected_types[i]) << "Regarding connector " << i + 1;
    }
}

void DispenserWithMultipleConnectors::assert_stop_request_counter_greater_or_equal(std::array<uint32_t, 4> expected) {
    for (int i = 0; i < 4; i++) {
        auto actual = power_stack_mock->get_stop_charge_request_counter(connector_configs[i].global_connector_number);

        EXPECT_GE(actual, expected[i]) << "Regarding connector " << i + 1;
    }
}

} // namespace dispenser_fixture

} // namespace user_acceptance_tests
